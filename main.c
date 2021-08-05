#include <stdint.h>

// PRCI MMIO Defines
#define PRCI_MMIO 0x10008000UL
#define HFROSCCFG 0x0
#define HFROSCEN (1 << 30)
#define PLLCFG 0x08
#define PLLREFSEL (1 << 17)
#define PLLBYPASS (1 << 18)
#define PLLSEL (1 << 16)

// GPIO MMIO Defines
#define GPIO_MMIO 0x10012000UL
#define IOF_EN 0x38
#define IOF_SEL 0x3C
#define GPIO_UART_MASK 0x00030000UL

// UART MMIO Defines
#define UART_MMIO 0x10013000UL
#define TXFIFO         0x00
#define RXFIFO         0x04
#define TXCTRL         0x08
#define RXCTRL         0x0c
#define IE             0x10
#define IP             0x14
#define DIV            0x18
#define TXEN           0x1

static inline void write_8(void* addr, uint8_t data) {
    *(volatile uint8_t*) addr = data;
}

static inline void write_32(void* addr, uint32_t data) {
    *(volatile uint32_t*) addr = data;
}

static inline void write_8_offset(void* addr, uint32_t offset, uint8_t data) {
    write_8((addr + offset), data);
}

static inline void write_32_offset(void* addr, uint32_t offset, uint32_t data) {
    write_32((addr + offset), data);
}

static inline uint32_t read_32(void* addr) {
    return *(volatile uint32_t*) addr;
}

static inline uint32_t read_32_offset(void* addr, uint32_t offset) {
    return read_32(addr + offset);
}

static inline void uart_write(uint8_t data) {
    while (read_32_offset(UART_MMIO, TXFIFO) & 0x80000000);

    write_8_offset(UART_MMIO, TXFIFO, data);
}

void uart_print(char* str) {
    while (*str) {
        uart_write(*str);
        if (*str == '\n') uart_write('\r');
        str++;
    }
}

void init_osc() {
    // Enable the High Frequency Ring OSCillator
    uint32_t hfrosccfg = read_32_offset(PRCI_MMIO, HFROSCCFG);
    hfrosccfg |= HFROSCEN;
    write_32_offset(PRCI_MMIO, HFROSCCFG, hfrosccfg);

    // Synchronize the PLL with the HFROSC
    uint32_t pllcfg = read_32_offset(PRCI_MMIO, PLLCFG);
    pllcfg |= PLLREFSEL;
    pllcfg |= PLLBYPASS;
    write_32_offset(PRCI_MMIO, PLLCFG, pllcfg);
    pllcfg |= PLLSEL;
    write_32_offset(PRCI_MMIO, PLLCFG, pllcfg);

    // Turn the HFROSC back off
    hfrosccfg &= ~HFROSCEN;
    write_32_offset(PRCI_MMIO, HFROSCCFG, hfrosccfg);
}

void init_uart() {
    uint32_t iof_en = read_32_offset(GPIO_MMIO, IOF_EN);
    uint32_t iof_sel = read_32_offset(GPIO_MMIO, IOF_SEL);

    iof_en |= GPIO_UART_MASK;
    iof_sel &= ~GPIO_UART_MASK;

    write_32_offset(GPIO_MMIO, IOF_EN, iof_en);
    write_32_offset(GPIO_MMIO, IOF_SEL, iof_sel);

    write_32_offset(UART_MMIO, DIV, 138);
    uint32_t txctrl = read_32_offset(UART_MMIO, TXCTRL);
    txctrl |= TXEN;
    txctrl |= (1 << 16);
    write_32_offset(UART_MMIO, TXCTRL, txctrl);

    uint32_t ie = read_32_offset(UART_MMIO, IE);
    ie |= 1;
    write_32_offset(UART_MMIO, IE, ie);

    // Busy loop
    volatile int i = 0;
    while (i++ < 1000000);
}

int main() {
    init_osc();
    init_uart();

    char* string = "Hello, UART!\nHave some printable ASCII:\n";
    uart_print(string);

    for (uint8_t i = 0x20; i < 0x80; i++) {
        uart_write(i);
        if (i % 0x10 == 0) { uart_write('\n'); uart_write('\r'); }
    }

    while (1);
    return 0;
}