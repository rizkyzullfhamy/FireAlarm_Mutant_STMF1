# FireAlarm Mutant STMF1 Project

### Langkah-langkah Install Software

1. Install VSCODE, dan install ekstension PlatformIO dan install
2. clone project
3. konfigurasi Menggunakan board SMT32F103
4. Compile project dan upload dengan STLINK

note untuk lebih detail seperti berikut : https://docs.platformio.org/en/stable/integration/ide/vscode.html

### Langkah-langkah Install Hardware

1. Gunakan perangkat STLINK V2 untuk upload program
2. konfigurasi hardware STM32F103 sebelum memulai upload untuk BOOT0 PIN agar dilakukan silang 0-1
3. konfigurasi pin STLINK uplaod (SWDIO,GND,SWCLK,3.3V) ke STM32F103
4. lakukan upload program

note untuk lebih detail seperti berikut : https://circuitdigest.com/microcontroller-projects/programming-stm32f103c8-board-using-usb-port
