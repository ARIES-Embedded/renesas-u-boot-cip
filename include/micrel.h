#ifndef _MICREL_H

#define MII_KSZ9021_EXT_COMMON_CTRL		0x100
#define MII_KSZ9021_EXT_STRAP_STATUS		0x101
#define MII_KSZ9021_EXT_OP_STRAP_OVERRIDE	0x102
#define MII_KSZ9021_EXT_OP_STRAP_STATUS		0x103
#define MII_KSZ9021_EXT_RGMII_CLOCK_SKEW	0x104
#define MII_KSZ9021_EXT_RGMII_RX_DATA_SKEW	0x105
#define MII_KSZ9021_EXT_RGMII_TX_DATA_SKEW	0x106
#define MII_KSZ9021_EXT_ANALOG_TEST		0x107
#ifdef CONFIG_IWG20M
/* Register operations */
#define MII_KSZ9031_MOD_REG                     0x0000
/* Data operations */
#define MII_KSZ9031_MOD_DATA_NO_POST_INC        0x4000
#define MII_KSZ9031_MOD_DATA_POST_INC_RW        0x8000
#define MII_KSZ9031_MOD_DATA_POST_INC_W         0xC000

#define MII_KSZ9031_EXT_MMD_ADD_MASK            0x1F
#define MII_KSZ9031_EXT_MMD_ADD1                0x01
#define MII_KSZ9031_EXT_MMD_ADD2                0x02

#define MII_KSZ9031_EXT_COMMON_CTRL             0x00
#define MII_KSZ9031_EXT_STRAP_STATUS            0x01
#define MII_KSZ9031_EXT_OP_STRAP_OVERRIDE       0x02
#define MII_KSZ9031_EXT_OP_STRAP_STATUS         0x03
#define MII_KSZ9031_EXT_RGMII_CTRL_SIG_SKEW     0x4
#define MII_KSZ9031_EXT_RGMII_RX_DATA_SKEW      0x5
#define MII_KSZ9031_EXT_RGMII_TX_DATA_SKEW      0x6
#define MII_KSZ9031_EXT_RGMII_CLOCK_SKEW        0x8

#endif
struct phy_device;
int ksz9021_phy_extended_write(struct phy_device *phydev, int regnum, u16 val);
int ksz9021_phy_extended_read(struct phy_device *phydev, int regnum);

#ifdef CONFIG_IWG20M
int ksz9031_phy_extended_write(struct phy_device *phydev, int devaddr,
                               int regnum, u16 mode, u16 val);
int ksz9031_phy_extended_read(struct phy_device *phydev, int devaddr,
                              int regnum, u16 mode);
#endif

#endif
