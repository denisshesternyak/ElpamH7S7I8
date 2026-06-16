import serial
import time
import struct
import crcmod
import argparse

# ======================= SETTINGS =======================
BAUDRATE = 115200
DEFAULT_TIMEOUT = 30
PACKET_SIZE = 256
MAX_RETRIES = 5
BASE_RETRY_DELAY = 0.1

PACKET_SOF = 0xAAAAAAAA
PACKET_EOF = 0x55555555

# Commands
CMD_READY         = 0x01
CMD_ACK           = 0xFF
CMD_NACK          = 0xEE
CMD_START_UPDATE  = 0x10
CMD_DATA_PACKET   = 0x11
CMD_END_UPDATE    = 0x12
CMD_STATISTIC     = 0x13
CMD_NMAGIC        = 0x14

crc16_func = crcmod.mkCrcFun(0x18005, rev=True, initCrc=0xFFFF, xorOut=0x0000)


class RS485Bootloader:
    def __init__(self, port, baud=BAUDRATE):
        self.ser = serial.Serial(
            port=port,
            baudrate=baud,
            timeout=1,
            write_timeout=1,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE
        )
        print(f"Port opened: {port} | {baud} baud")

    def calc_crc(self, data: bytes) -> int:
        return crc16_func(data)

    def send_packet(self, cmd: int, data: bytes = b''):
        length = len(data)
        packet = struct.pack('<IBH', PACKET_SOF, cmd, length)
        if data:
            packet += data
        crc = self.calc_crc(packet)
        packet += struct.pack('<HI', crc, PACKET_EOF)

        self.ser.write(packet)
        self.ser.flush()
        time.sleep(0.009)

    def receive_packet(self, timeout=None):
        if timeout is not None:
            self.ser.timeout = timeout

        raw = self.ser.read_until(struct.pack('<I', PACKET_EOF))

        if len(raw) < 11 or raw[:4] != struct.pack('<I', PACKET_SOF):
            return None, None, None

        cmd = raw[4]
        length = struct.unpack('<H', raw[5:7])[0]

        expected_len = 13 + length  # SOF+CMD+LEN+DATA+CRC+EOF
        if len(raw) != expected_len:
            return None, None, None

        payload = raw[7:7 + length]
        received_crc = struct.unpack('<H', raw[7 + length:9 + length])[0]

        calculated_crc = self.calc_crc(raw[:7 + length])

        if calculated_crc != received_crc:
            print("CRC Error!")
            return None, None, None

        return cmd, payload, raw

    def wait_ready(self, timeout_sec=DEFAULT_TIMEOUT):
        print(f"Waiting for READY command from bootloader ({timeout_sec} sec)...")
        start_time = time.time()
        
        while time.time() - start_time < timeout_sec:
            cmd, payload, _ = self.receive_packet(0.5)
            if cmd == CMD_READY:
                print("Bootloader ready for firmware update")
                return True
        print("Timeout: bootloader did not respond with READY")
        return False
        
    def send_with_retry(self, cmd: int, data: bytes = b'',
                        timeout: float = 3.0, packet_name: str = "packet") -> bool:

        for attempt in range(MAX_RETRIES):
            self.send_packet(cmd, data)
            response_cmd, payload, _ = self.receive_packet(timeout)
            
            if response_cmd == CMD_ACK:
                if attempt > 0:
                    print(f"{packet_name} OK (attempt {attempt+1})")
                return True
            elif response_cmd == CMD_NACK:
                print(f"{packet_name} NACK (attempt {attempt+1})")
            elif response_cmd == CMD_NMAGIC:
                print(f"{packet_name} Firmware is invalid!")
                break
            else:
                print(f"{packet_name} no response (attempt {attempt+1})")

            if attempt < MAX_RETRIES - 1:
                time.sleep(BASE_RETRY_DELAY * (2 ** attempt))

        print(f"{packet_name} failed after {MAX_RETRIES} attempts")
        return False

    def send_firmware(self, filepath: str):
        with open(filepath, 'rb') as f:
            firmware = f.read()
            
        total_size = len(firmware)

        print("[1/2] Sending data packets...")
        for i in range(0, total_size, PACKET_SIZE):
            chunk = firmware[i:i + PACKET_SIZE]
            packet_num = i // PACKET_SIZE + 1

            if not self.send_with_retry(CMD_DATA_PACKET, chunk, timeout=3,
                                      packet_name=f"Packet {packet_num}"):
                print("Aborting due to critical error.")
                return False

            if packet_num % 16 == 0 or i + PACKET_SIZE >= total_size:
                progress = min(100, (i + PACKET_SIZE) * 100 // total_size)
                print(f"Progress: {progress:3d}%")

        print("\n[2/2] Sending End of Update...")
        if not self.send_with_retry(CMD_END_UPDATE, b'', timeout=8, packet_name="End Update"):
            return False

        print("\nWaiting for statistics from MCU...")
        cmd, payload, _ = self.receive_packet(timeout=10)

        if cmd == CMD_STATISTIC and payload and len(payload) >= 12:
            packet_count = struct.unpack('<I', payload[0:4])[0]
            error_count = struct.unpack('<I', payload[4:8])[0]
            total_bytes = struct.unpack('<I', payload[8:12])[0]

            print("="*50)
            print("📊 STATISTICS FROM MCU:")
            print(f"   Packets received : {packet_count}")
            print(f"   Errors           : {error_count}")
            print(f"   Total bytes      : {total_bytes}")
            print(f"   Success rate     : {100 if error_count == 0 else (packet_count / (packet_count + error_count)) * 100:.1f}%")
            print("="*50)
            return True
        else:
            print("Statistics not received or invalid")
            return True

# ======================= Main =======================
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="STM32H7 RS485 Firmware Updater")
    parser.add_argument("port", help="COM port")
    parser.add_argument("firmware", help="Firmware .bin file")
    parser.add_argument("-b", "--baud", type=int, default=BAUDRATE, help="Baud rate")
    parser.add_argument("-t", "--timeout", type=int, default=DEFAULT_TIMEOUT, help="READY timeout (sec)")

    args = parser.parse_args()

    updater = RS485Bootloader(port=args.port, baud=args.baud)

    if updater.wait_ready(timeout_sec=args.timeout):
        updater.send_firmware(args.firmware)
    else:
        print("Bootloader did not respond with READY command.")