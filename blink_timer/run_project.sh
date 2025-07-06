#!/bin/bash

PORT="/dev/ttyUSB0"
BAUD_RATE=115200
FLASH_SIZE="4MB"
FLASH_MODE="dio"
FLASH_FREQ="40m"

echo "=== Configurações ==="
echo "Porta: $PORT"
echo "Baud Rate: $BAUD_RATE"
echo "GPIO LED: 2 (fixo)"
echo "====================="

echo "Limpando build anterior..."
rm -rf build

echo "Construindo projeto..."
idf.py build

if [ $? -ne 0 ]; then
    echo "❌ Falha na construção do projeto!"
    exit 1
fi

echo "Gravando no dispositivo..."
esptool.py --chip esp32 --port $PORT --baud $BAUD_RATE \
           --before default_reset --after hard_reset write_flash \
           --flash_mode $FLASH_MODE --flash_freq $FLASH_FREQ --flash_size $FLASH_SIZE \
           0x1000 build/bootloader/bootloader.bin \
           0x8000 build/partition_table/partition-table.bin \
           0x10000 build/blink_timer.bin

if [ $? -ne 0 ]; then
    echo "❌ Falha na gravação!"
    exit 1
fi

echo "✅ Gravação concluída com sucesso!"
