import argparse
import struct
import crcmod

MAGIC_NUMBER = 0xDEADBEEF

crc16_func = crcmod.mkCrcFun(0x18005, rev=True, initCrc=0xFFFF, xorOut=0x0000)

def generate_key(salt: bytes, length: int) -> bytes:
    key = bytearray()
    while len(key) < length:
        key.extend(salt)
    return key[:length]
    
def calc_crc(data: bytes) -> int:
        return crc16_func(data)

def protect_firmware(input_file: str, output_file: str, salt: str):
    salt_bytes = salt.encode('utf-8')
    
    with open(input_file, 'rb') as f:
        firmware = f.read()
    
    total_size = len(firmware)
    fw_crc16 = calc_crc(firmware)

    print(f"File: {total_size/1024:.1f} KB | CRC16 = 0x{fw_crc16:04X}\n")
    header = struct.pack('<III', MAGIC_NUMBER, total_size, fw_crc16)
    
    data_with_magic = header + firmware
    
    key = generate_key(salt_bytes, len(data_with_magic))
    encrypted = bytes(a ^ b for a, b in zip(data_with_magic, key))
    
    with open(output_file, 'wb') as f:
        f.write(encrypted)
    
    print(f"Protected firmware created: {output_file} ({len(encrypted)} bytes)")
    print(f"Used SALT: {salt}")
    print(f"Magic number: 0x{MAGIC_NUMBER:08X}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Protect firmware using XOR")
    parser.add_argument("input", help="Source .bin file")
    parser.add_argument("salt", help="Salt for key generation")
    parser.add_argument("-o", "--output", default="firmware_protected.bin", help="Output file")
    parser.add_argument("-m", "--magic", default="0xDEADBEEF", help="Magic number (hex)")
    args = parser.parse_args()
    
    if args.magic:
        MAGIC_NUMBER = int(args.magic, 16)
    
    protect_firmware(args.input, args.output, args.salt)