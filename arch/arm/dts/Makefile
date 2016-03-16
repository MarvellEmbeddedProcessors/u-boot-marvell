dtb-$(CONFIG_MACH_SUN7I) +=  sun7i-a20-pcduino3.dtb
dtb-$(CONFIG_S5PC100) += s5pc1xx-smdkc100.dtb
dtb-$(CONFIG_S5PC110) += s5pc1xx-goni.dtb
dtb-$(CONFIG_EXYNOS4) += exynos4210-origen.dtb \
	exynos4210-smdkv310.dtb \
	exynos4210-universal_c210.dtb \
	exynos4210-trats.dtb \
	exynos4412-trats2.dtb \
	exynos4412-odroid.dtb

dtb-$(CONFIG_EXYNOS5) += exynos5250-arndale.dtb \
	exynos5250-snow.dtb \
	exynos5250-smdk5250.dtb \
	exynos5420-smdk5420.dtb \
	exynos5420-peach-pit.dtb \
	exynos5800-peach-pi.dtb \
	exynos5422-odroidxu3.dtb
dtb-$(CONFIG_TEGRA) += tegra20-harmony.dtb \
	tegra20-medcom-wide.dtb \
	tegra20-paz00.dtb \
	tegra20-plutux.dtb \
	tegra20-seaboard.dtb \
	tegra20-tec.dtb \
	tegra20-trimslice.dtb \
	tegra20-ventana.dtb \
	tegra20-whistler.dtb \
	tegra20-colibri_t20_iris.dtb \
	tegra30-apalis.dtb \
	tegra30-beaver.dtb \
	tegra30-cardhu.dtb \
	tegra30-colibri.dtb \
	tegra30-tec-ng.dtb \
	tegra114-dalmore.dtb \
	tegra124-jetson-tk1.dtb \
	tegra124-nyan-big.dtb \
	tegra124-venice2.dtb
dtb-$(CONFIG_ARCH_UNIPHIER) += \
	uniphier-ph1-sld3-ref.dtb \
	uniphier-ph1-pro4-ref.dtb \
	uniphier-ph1-ld4-ref.dtb \
	uniphier-ph1-sld8-ref.dtb
dtb-$(CONFIG_ZYNQ) += zynq-zc702.dtb \
	zynq-zc706.dtb \
	zynq-zed.dtb \
	zynq-zybo.dtb \
	zynq-microzed.dtb \
	zynq-zc770-xm010.dtb \
	zynq-zc770-xm012.dtb \
	zynq-zc770-xm013.dtb
dtb-$(CONFIG_AM33XX) += am335x-boneblack.dtb

dtb-$(CONFIG_SOCFPGA) += socfpga_cyclone5_socrates.dtb

# If Multi-FDT feature is enabled, all the DTB files that are compiled
# will be linked into SPL, the dtb size increase the SPL size.
dtb-$(CONFIG_TARGET_ARMADA_38X) += armada-385-db.dtb
ifdef CONFIG_TARGET_ARMADA_3700
ifndef CONFIG_DEVEL_BOARD
dtb-y += armada-3700-customer0.dtb
endif #CONFIG_DEVEL_BOARD
dtb-$(CONFIG_DEVEL_BOARD) += armada-3700-db.dtb \
			     armada-3700-db-sgmii1.dtb
dtb-$(CONFIG_PALLADIUM) += armada-3700-palladium.dtb
endif #CONFIG_TARGET_ARMADA_3700

ifdef CONFIG_TARGET_ARMADA_8K
dtb-$(CONFIG_PALLADIUM) += apn-806-pd.dtb \
			   apn-806-pd-a0.dtb \
			   armada-70x0-pd.dtb
dtb-$(CONFIG_MVEBU_SPL_DDR_OVER_PCI_SUPPORT) += apn-806-db-dop.dtb \
						armada-7040-rz-db_dop.dtb
dtb-$(CONFIG_DEVEL_BOARD) += apn-806-db.dtb \
			     armada-80x0-db.dtb

ifdef CONFIG_AP806_Z_SUPPORT
dtb-$(CONFIG_DEVEL_BOARD) += armada-7040-rz-db_nand.dtb \
			     armada-7040-rz-db_router.dtb \
			     armada-7040-rz-db_all_pci.dtb \
			     armada-7040-rz-db_rgmii_only.dtb \
			     armada-7040-rz-db_kr.dtb \
			     armada-7040-rz-db_rxaui_pci.dtb \
			     armada-7040-rz-db_rxaui.dtb
else
dtb-$(CONFIG_DEVEL_BOARD) += armada-70x0-db_router.dtb
endif
endif #CONFIG_TARGET_ARMADA_8K

targets += $(dtb-y)

# alignment fdt file to 8k. this is helpful to support multi fdt.
# This alignment allows selecting DT files easily by skipping over a predefined DT size.
DTC_FLAGS += -R 4 -S 0x1C00

PHONY += dtbs
dtbs: $(addprefix $(obj)/, $(dtb-y))
	@:

clean-files := *.dtb
