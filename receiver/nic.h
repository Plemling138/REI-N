#define RTL8019_ADDR 0x200000

#define NIC_CR			(*(volatile uint8 *)(RTL8019_ADDR + 0x00))
#define NIC_RDMAP		(*(volatile uint8 *)(RTL8019_ADDR + 0x10))
#define NIC_RP			(*(volatile uint8 *)(RTL8019_ADDR + 0x18))

/*Page0*/
#define NIC_PSTART	(*(volatile uint8 *)(RTL8019_ADDR + 0x01))
#define NIC_PSTOP		(*(volatile uint8 *)(RTL8019_ADDR + 0x02))
#define NIC_BNRY		(*(volatile uint8 *)(RTL8019_ADDR + 0x03))
#define NIC_TPSR		(*(volatile uint8 *)(RTL8019_ADDR + 0x04))
#define NIC_TBCR0		(*(volatile uint8 *)(RTL8019_ADDR + 0x05))
#define NIC_TBCR1		(*(volatile uint8 *)(RTL8019_ADDR + 0x06))
#define NIC_ISR			(*(volatile uint8 *)(RTL8019_ADDR + 0x07))
#define NIC_RSAR0		(*(volatile uint8 *)(RTL8019_ADDR + 0x08))
#define NIC_RSAR1		(*(volatile uint8 *)(RTL8019_ADDR + 0x09))
#define NIC_RBCR0		(*(volatile uint8 *)(RTL8019_ADDR + 0x0A))
#define NIC_RBCR1		(*(volatile uint8 *)(RTL8019_ADDR + 0x0B))
#define NIC_RCR			(*(volatile uint8 *)(RTL8019_ADDR + 0x0C))
#define NIC_TCR			(*(volatile uint8 *)(RTL8019_ADDR + 0x0D))
#define NIC_DCR			(*(volatile uint8 *)(RTL8019_ADDR + 0x0E))
#define NIC_IMR			(*(volatile uint8 *)(RTL8019_ADDR + 0x0F))

/*Page1*/
#define NIC_PAR0		(*(volatile uint8 *)(RTL8019_ADDR + 0x01))
#define NIC_PAR1		(*(volatile uint8 *)(RTL8019_ADDR + 0x02))
#define NIC_PAR2		(*(volatile uint8 *)(RTL8019_ADDR + 0x03))
#define NIC_PAR3		(*(volatile uint8 *)(RTL8019_ADDR + 0x04))
#define NIC_PAR4		(*(volatile uint8 *)(RTL8019_ADDR + 0x05))
#define NIC_PAR5		(*(volatile uint8 *)(RTL8019_ADDR + 0x06))
#define NIC_CURR		(*(volatile uint8 *)(RTL8019_ADDR + 0x07))
#define NIC_MAR0		(*(volatile uint8 *)(RTL8019_ADDR + 0x08))
#define NIC_MAR1		(*(volatile uint8 *)(RTL8019_ADDR + 0x09))
#define NIC_MAR2		(*(volatile uint8 *)(RTL8019_ADDR + 0x0A))
#define NIC_MAR3		(*(volatile uint8 *)(RTL8019_ADDR + 0x0B))
#define NIC_MAR4		(*(volatile uint8 *)(RTL8019_ADDR + 0x0C))
#define NIC_MAR5		(*(volatile uint8 *)(RTL8019_ADDR + 0x0D))
#define NIC_MAR6		(*(volatile uint8 *)(RTL8019_ADDR + 0x0E))
#define NIC_MAR7		(*(volatile uint8 *)(RTL8019_ADDR + 0x0F))


/* Page3 (for RTL8019) */
#define RTL8019_9346CR  (*(volatile uint8 *)(RTL8019_ADDR + 0x01))
#define RTL8019_BPAGE   (*(volatile uint8 *)(RTL8019_ADDR + 0x02))
#define RTL8019_CONFIG0 (*(volatile uint8 *)(RTL8019_ADDR + 0x03))
#define RTL8019_CONFIG1 (*(volatile uint8 *)(RTL8019_ADDR + 0x04))
#define RTL8019_CONFIG2 (*(volatile uint8 *)(RTL8019_ADDR + 0x05))
#define RTL8019_CONFIG3 (*(volatile uint8 *)(RTL8019_ADDR + 0x06))
#define RTL8019_TEST    (*(volatile uint8 *)(RTL8019_ADDR + 0x07))
#define RTL8019_CSNSAV  (*(volatile uint8 *)(RTL8019_ADDR + 0x08))
#define RTL8019_HLTCLK  (*(volatile uint8 *)(RTL8019_ADDR + 0x09))
#define RTL8019_INTR    (*(volatile uint8 *)(RTL8019_ADDR + 0x0b))
#define RTL8019_CONFIG4 (*(volatile uint8 *)(RTL8019_ADDR + 0x0d))

#define NIC_CR_P0       (0 << 6)
#define NIC_CR_P1       (1 << 6)
#define NIC_CR_P2       (2 << 6)
#define NIC_CR_P3       (3 << 6)
#define NIC_CR_RD_ABORT (4 << 3)
#define NIC_CR_RD_WRITE (2 << 3)
#define NIC_CR_RD_READ  (1 << 3)
#define NIC_CR_TXP      (1 << 2)
#define NIC_CR_STA      (1 << 1)
#define NIC_CR_STP      (1 << 0)

#define NIC_ISR_RDC     (1 << 6)

#define NIC_RCR_MON     (1 << 5)
#define NIC_RCR_PRO     (1 << 4)
#define NIC_RCR_AM      (1 << 3)
#define NIC_RCR_AB      (1 << 2)

#define NIC_TCR_ELB     (2 << 1)
#define NIC_TCR_ILB     (1 << 1)
#define NIC_TCR_NORMAL  (0 << 1)

#define NIC_DCR_F1      (1 << 6)
#define NIC_DCR_F0      (1 << 5)
#define NIC_DCR_LS      (1 << 3)
#define NIC_DCR_BOS     (1 << 1)

#define NIC_TP_START 0x40
#define NIC_RP_START 0x46
#define NIC_RP_STOP  0x80

