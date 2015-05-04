#include <config.h>
#include <common.h>
#include "mvCommon.h"
#include <command.h>
#include <net.h>
#include <environment.h>

#ifdef CONFIG_CMD_SOURCE
#if defined(MV_INCLUDE_USB)
#include <usb.h>
#endif

#include <fs.h>

#define	DESTINATION_STRING	10

#if defined(CONFIG_CMD_NAND)
#include <nand.h>
extern nand_info_t nand_info[];       /* info for NAND chips */
#endif

#ifdef CONFIG_CMD_SF
#include <spi_flash.h>
extern struct spi_flash *flash;
#endif

#ifdef CONFIG_CMD_FLASH
#include <flash.h>
extern flash_info_t flash_info[];       /* info for FLASH chips */
#endif

#include <image.h>
#include <malloc.h>

int dest_flash; /* destination flash to burn the script (0 = SPI, 1 = NAND) */

#ifdef MV_INCLUDE_USB
/*******************************************************************************
* load_from_usb
*
* DESCRIPTION:
* Load script file from usb device
*
* INPUT:
*	file_name	- img file name.
*
* OUTPUT:
*	None.
*
* RETURN:
*	- file size on success
*	- 0, otherwise
*
*******************************************************************************/
static int load_from_usb(const char *file_name)
{
	const char *addr_str;
	unsigned long addr;
	int filesize = 0;

	usb_stop();
	printf("(Re)start USB...\n");

	if (usb_init() < 0) {
		printf("usb_init failed\n");
		return 0;
	}

	/* try to recognize storage devices immediately */
	/* the parameter '1', tells the function to reports to the
	 * user while scanning for device */
	if (-1 == usb_stor_scan(1)) {
		printf("USB storage device not found\n");
		return 0;
	}

	/* always load from usb 0 */
	if (fs_set_blk_dev("usb", "0", FS_TYPE_ANY)) {
		printf("USB 0 not found\n");
		return 0;
	}

	addr_str = getenv("loadaddr");
	if (addr_str != NULL)
		addr = simple_strtoul(addr_str, NULL, 16);
	else
		addr = CONFIG_SYS_LOAD_ADDR;

	filesize = fs_read(file_name, addr, 0, 0);
	return filesize;
}
#endif

/*******************************************************************************
* fetch_script_file
*
* DESCRIPTION:
* Load script file into ram from external device: tftp, usb
*
* INPUT:
*	loadfrom	- specifies the source device:
*			  0 - TFTP, 1 - USB
*
* OUTPUT:
*	None.
*
* RETURN:
*	- file size on success
*	- 0, otherwise
*
*******************************************************************************/
static int fetch_script_file(int loadfrom)
{
	int filesize = 0;
	switch (loadfrom) {
#ifdef MV_INCLUDE_USB
	case 1:
		filesize = load_from_usb(BootFile);
		if (filesize <= 0) {
			printf("Failed to read file %s\n", BootFile);
			return 0;
		}
		break;
#endif
	case 0:
		filesize = NetLoop(TFTPGET);
		printf("Checking file size:");
		if (filesize == -1) {
			printf("\t[Fail]\n");
			return 0;
		}
		break;
	default:
		return 0;
	}

	return filesize;
}

/*******************************************************************************
* fetch_mvsource_cmd_args
*
* DESCRIPTION:
* fetch command arguments from argv and return them in variables
*
* INPUT:
*	argc		- command arguments' number
*	argv		- command arguments' values
*
* OUTPUT:
*	command		- command to run (0 = burn, 1 = run)
*	offset		- offset in flash to burn the script
*	loadfrom	- source interface (0 = TFTP, 1 = USB)
*	dest		- destination flash if multiple flashes supported
*			  (0 = SPI, 1 = NAND)
*
* RETURN:
*	- MV_OK on success
*	- MV_FAIL, otherwise
*
*******************************************************************************/
static MV_STATUS fetch_mvsource_cmd_args(int argc, char * const argv[],
		int *command, MV_U32 *offset, int *loadfrom, int *dest) {
	int i;
	const char *default_name = "script.img";

	*command = -1;		/* 0 = burn; 1 = run; -1 = invalid command */
	*offset = 0x300000;
	*loadfrom = 0;		/* 0 = TFTP; 1 = USB */
	*dest = 0;		/* 0 = SPI; 1 = NAND */

	/* fetch destination interface (if exists), else use SPI as default */
	for (i = 0; i < argc; ++i) {
		if (strcmp(argv[i], "spi") == 0) {
			*dest = 0;
			argc--;
			break;
		} else if (strcmp(argv[i], "nand") == 0) {
			*dest = 1;
			argc--;
			break;
		}
	}

	if (argc < 2) { /* empty command is unacceptable */
		printf("Error: too few arguments\n");
		return MV_FAIL;
	}

	/* skip main command name */
	argc--;
	argv++;
	if (strcmp(argv[0], "burn") == 0) {
		*command = 0;
		/* only burn command have filename as 1st parameter */
		if (argc > 1) {
			strncpy(BootFile, argv[1], strlen(argv[1]));
			argc--;
			argv++;
		} else {
			strncpy(BootFile, default_name, strlen(default_name));
			printf("using default filename \"%s\"\n", default_name);
		}
	} else if (strcmp(argv[0], "run") == 0)
		*command = 1;
	else {
		printf("Error: unknown command\n");
		return MV_FAIL;
	}

	/* skip internal command name ('burn' or 'run') */
	argc--;
	argv++;
	switch (argc) {
	case 2:
		if (strcmp(argv[1], "tftp") == 0)
			*loadfrom = 0;
		else if (strcmp(argv[1], "usb") == 0)
			*loadfrom = 1;
		else
			return MV_FAIL;
	case 1:
		*offset = simple_strtoul(argv[0], NULL, 16);
	default:
		break;
	}
	return MV_OK;
}

#if defined(MV_INCLUDE_SPI)
/*******************************************************************************
* init_spi
*
* DESCRIPTION:
* init spi flash if not initiated
*
* INPUT:
*	None.
*
* OUTPUT:
*	None.
*
* RETURN:
*	0	on success
*	-1	otherwise
*
*******************************************************************************/
static int init_spi(void){
	if (!flash) {
		flash = spi_flash_probe(CONFIG_ENV_SPI_BUS, CONFIG_ENV_SPI_CS,
								CONFIG_SF_DEFAULT_SPEED, CONFIG_SF_DEFAULT_MODE);
		if (!flash) {
			printf("Failed to probe SPI Flash\n");
			return -1;
		}
	}
	return 0;
}

/*******************************************************************************
* spi_burn_script
*
* DESCRIPTION:
* burn script image to the SPI flash
*
* INPUT:
*	offset		- offset in flash to burn the script
*	loadfrom	- source interface to fetch the file (0 = TFTP, 1 = USB)
*
* OUTPUT:
*	None.
*
* RETURN:
*	None.
*
*******************************************************************************/
static void spi_burn_script(MV_U32 offset, int loadfrom)
{
	int filesize = 0;
	MV_U32 ret = 0;
	load_addr = CONFIG_SYS_LOAD_ADDR;
	u32 sector_size, aligned_size;

	if (init_spi())
		return;

	sector_size =  flash->sector_size;

	/* verify requested source is valid */
	filesize = fetch_script_file(loadfrom);
	if (filesize <= 0)
		return;

	aligned_size = ((filesize + (sector_size - 1)) & (~(sector_size - 1)));
	printf("Erasing 0x%x - 0x%x: ", offset, offset + aligned_size);
	spi_flash_erase(flash, offset, aligned_size);
	printf("\t\t[Done]\n");

	printf("Writing image to flash:");
	ret = spi_flash_write(flash, offset, filesize, (const void *)load_addr);

	if (ret)
		printf("\t\t[Err!]\n");
	else
		printf("\t\t[Done]\n");
}
#endif /* MV_INCLUDE_SPI */

#if defined(MV_NAND)
/*******************************************************************************
* spi_burn_script
*
* DESCRIPTION:
* burn script image to the NAND flash
*
* INPUT:
*	offset		- offset in flash to burn the script
*	loadfrom	- source interface to fetch the file (0 = TFTP, 1 = USB)
*
* OUTPUT:
*	None.
*
* RETURN:
*	None.
*
*******************************************************************************/
static void nand_burn_script(MV_U32 offset, int loadfrom)
{
	int filesize = 0;
	MV_U32 ret = 0, aligned_size;
	nand_info_t *nand = &nand_info[0];
	size_t erasesize = nand_info[0].erasesize;

	if (nand_block_isbad(&nand_info[0], offset)) {
		printf("Failed to burn: Bad block at offset %X\n", offset);
		return;
	}

	/* verify requested source is valid */
	filesize = fetch_script_file(loadfrom);
	if (filesize <= 0)
		return;

	aligned_size = ((filesize + (erasesize - 1)) & (~(erasesize - 1)));
	printf("\t[Done]\n");
	printf("Erasing 0x%x - 0x%x: ", offset, offset + aligned_size);
	nand_erase(nand, offset, aligned_size);
	printf("\t[Done]\n");

	printf("Writing image to NAND:");
	ret = nand_write(nand, offset, (size_t *)&filesize, (u_char *)load_addr);
	if (ret)
		printf("\t[Fail]\n");
	else
		printf("\t[Done]\n");
}
#endif /* defined(MV_NAND) */

/*******************************************************************************
* is_magic_valid
*
* DESCRIPTION:
* checks if the magic value in script header is valid
*
* INPUT:
*	header		- script header
*
* RETURN:
*	MV_TRUE if magic value in header is valid
*	MV_FALSE otherwise
*
*******************************************************************************/
MV_BOOL is_magic_valid(image_header_t *header)
{
	/* check if image type is IMAGE_FORMAT_LEGACY */
	if (be32_to_cpu(header->ih_magic) != IH_MAGIC)
		return MV_FALSE;

	return MV_TRUE;
}

/*******************************************************************************
* get_image_size
*
* DESCRIPTION:
* fetch script size (in bytes) from script header
*
* INPUT:
*	offset		- script offset in flash
*
* OUTPUT:
*	size		- script size (in bytes)
*
* RETURN:
*	MV_OK on success fetching the size
*	MV_FAIL otherwise
*
*******************************************************************************/
MV_STATUS get_image_size(MV_U32 offset, MV_U32 *size)
{
	size_t buff_size = sizeof(image_header_t);
	image_header_t header;
#if defined(MV_INCLUDE_SPI)
	if (init_spi())
		return MV_FAIL;
#endif
#if defined(MV_INCLUDE_SPI) && defined(MV_NAND)
		if (dest_flash == 0)
			spi_flash_read(flash, offset, buff_size , &header);
		else if (dest_flash == 1)
			nand_read(&nand_info[0], offset, &buff_size, (u_char *)&header);
#elif defined(MV_INCLUDE_SPI)
	spi_flash_read(flash, offset, buff_size , &header);
#elif defined(MV_NAND)
	nand_read(&nand_info[0], offset, &buff_size, (u_char *)&header);
#else
	return MV_FAIL;
#endif
	if (is_magic_valid(&header) == MV_FALSE)
		return MV_FAIL;

	*size = be32_to_cpu(header.ih_size) + sizeof(image_header_t);
	return MV_OK;
}

/*******************************************************************************
* get_block_size
*
* DESCRIPTION:
* get block size of flash
*
* INPUT:
*	None.
*
* OUTPUT:
*	None.
*
* RETURN:
*	block size, on success
*	0, otherwise
*
*******************************************************************************/
MV_U32 get_block_size(void)
{
#if defined(MV_INCLUDE_SPI) && defined(MV_NAND)
		if (dest_flash == 0)
			return flash->sector_size;
		else if (dest_flash == 1)
			return nand_info[0].erasesize;
		else
			return 0;
#elif defined(MV_INCLUDE_SPI)
	return flash->sector_size;

#elif defined(MV_NAND)
	return nand_info[0].erasesize;
#else
	return 0;
#endif
}

int mvsource_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int command, loadfrom;
	MV_U32 offset;
	char *buffer;
	char buffer_address_str[30];	/* to pass to 'source' argument */
	MV_U32 size;
	char *source_argv[2];

	if (fetch_mvsource_cmd_args(argc, argv, &command,
				&offset, &loadfrom, &dest_flash) != MV_OK)
		return 1;


	if (command == 0) { /* 0 = burn */
#if defined(MV_INCLUDE_SPI) && defined(MV_NAND)
		if (dest_flash == 0)
			spi_burn_script(offset, loadfrom);
		else if (dest_flash == 1)
			nand_burn_script(offset, loadfrom);
#elif defined(MV_INCLUDE_SPI)
		spi_burn_script(offset, loadfrom);
#elif defined(MV_NAND)
		nand_burn_script(offset, loadfrom);
#endif
	} else if (command == 1) { /* 1 = run */
		if (get_image_size(offset, &size) != MV_OK) {
			printf("Error: Failed to fetch script size\n");
			return 1;
		} else {
			buffer = (char *)malloc(size);
			if (!buffer) {
				printf("Error: Failed to allocate memory\n");
				return 1;
			}
		}
#if defined(MV_INCLUDE_SPI) && defined(MV_NAND)
		if (dest_flash == 0)
			spi_flash_read(flash, offset, size , buffer);
		else if (dest_flash == 1)
			nand_read(&nand_info[0], offset, &size, (u_char *)buffer);
#elif defined(MV_INCLUDE_SPI)
		spi_flash_read(flash, offset, size , buffer);
#elif defined(MV_NAND)
		nand_read(&nand_info[0], offset, &size, (u_char *)buffer);
#else
		return 1;
#endif
		sprintf(buffer_address_str, "%lx", (unsigned long)buffer);
		source_argv[0] = "source";
		source_argv[1] = buffer_address_str;
		do_source(cmdtp, 0, 2, source_argv);
		free(buffer);
	}
	return 0;
}

U_BOOT_CMD(
		mvsource,      5,     1,      mvsource_cmd,
		"mvsource	- Burn a script image on flash device.\n",
		"burn [file-name [offset [src_interface]]] [dst_flash]"
		"\n\tBurn script <file-name> to flash at <offset> from <src_interface> \n"
		"mvsource run [offset]"
		"\n\tRun script from flash at <offset>\n"
		"\nArguments (optional):\n"
		"\t- file-name: script file name\t\t\t(default = script.bin)\n"
		"\t- offset: script address in flash\t\t(default = 300000)\n"
		"\t- src_interface: script source: tftp/usb\t(default = tftp)\n"
		"\t- dst_flash: flash destination: spi/nand\t(default = spi)\n"
		"Examples:\n"
		"\tmvsource burn script.img 300000 tftp spi\n"
		"\tmvsource burn\n"
		"\tmvsource run 300000\n"
		"\tmvsource run\n"
		);

#endif /* CONFIG_CMD_SOURCE */
