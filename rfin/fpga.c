#include "fpga.h"
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static int __osc_fpga_cleanup_mem(void);
//-----------------------------------------------------------------------------
static osc_fpga_reg_mem_t *g_osc_fpga_reg_mem = NULL;
/* @brief Pointer to data buffer where signal on channel A is captured.  */
static uint32_t           *g_osc_fpga_cha_mem = NULL;
/* @brief Pointer to data buffer where signal on channel B is captured.  */
static uint32_t           *g_osc_fpga_chb_mem = NULL;
/* @brief The memory file descriptor used to mmap() the FPGA space. */
static int                 g_osc_fpga_mem_fd = -1;

//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------*leanup access to FPGA memory buffers
/*
 * Function optionally cleanups access to FPGA memory buffers, i.e. if access
 * has already been established it unmaps logical memory regions and close apparent
 * file descriptor.
 *
 * @retval  0 Success
 * @retval -1 Failure, error message is printed on standard error device
 */
static int __osc_fpga_cleanup_mem(void)
{
    /* optionally unmap memory regions  */
    if (g_osc_fpga_reg_mem) {
        if (munmap(g_osc_fpga_reg_mem, OSC_FPGA_BASE_SIZE) < 0) {
            fprintf(stderr, "munmap() failed: %s\n", strerror(errno));
            return -1;
        }
        /* ...and update memory pointers */
        g_osc_fpga_reg_mem = NULL;
        g_osc_fpga_cha_mem = NULL;
        g_osc_fpga_chb_mem = NULL;
    }

    /* optionally close file descriptor */
    if(g_osc_fpga_mem_fd >= 0) {
        close(g_osc_fpga_mem_fd);
        g_osc_fpga_mem_fd = -1;
    }

    return 0;
}

/**
 * @brief Initialize interface to Oscilloscope FPGA module
 *
 * Function first optionally cleanups previously established access to Oscilloscope
 * FPGA module. Afterwards a new connection to the Memory handler is instantiated
 * by opening file descriptor over /dev/mem device. Access to Oscilloscope FPGA module
 * is further provided by mapping memory regions through resulting file descriptor.
 *
 * @retval  0 Success
 * @retval -1 Failure, error message is printed on standard error device
 *
 */
int osc_fpga_init(void)
{
    void *page_ptr;
    long page_addr, page_off, page_size = sysconf(_SC_PAGESIZE);

    /* If maybe needed, cleanup the FD & memory pointer */
    if(__osc_fpga_cleanup_mem() < 0)
        return -1;

    g_osc_fpga_mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if(g_osc_fpga_mem_fd < 0) {
        fprintf(stderr, "open(/dev/mem) failed: %s\n", strerror(errno));
        return -1;
    }

    page_addr = OSC_FPGA_BASE_ADDR & (~(page_size-1));
    page_off  = OSC_FPGA_BASE_ADDR - page_addr;

    page_ptr = mmap(NULL, OSC_FPGA_BASE_SIZE, PROT_READ | PROT_WRITE,
                          MAP_SHARED, g_osc_fpga_mem_fd, page_addr);
    if((void *)page_ptr == MAP_FAILED) {
        fprintf(stderr, "mmap() failed: %s\n", strerror(errno));
        __osc_fpga_cleanup_mem();
        return -1;
    }
    g_osc_fpga_reg_mem = page_ptr + page_off;
    g_osc_fpga_cha_mem = (uint32_t *)g_osc_fpga_reg_mem + 
        (OSC_FPGA_CHA_OFFSET / sizeof(uint32_t));
    g_osc_fpga_chb_mem = (uint32_t *)g_osc_fpga_reg_mem + 
        (OSC_FPGA_CHB_OFFSET / sizeof(uint32_t));

    return 0;
}

/*----------------------------------------------------------------------------*/
/**
 * @brief Reset write state machine
 *
 * @retval 0 Success, never fails
 */
int osc_fpga_reset(void)
{
	printf ("'g_osc_fpga_reg_mem'=%p\n", g_osc_fpga_reg_mem);
    g_osc_fpga_reg_mem->conf |= OSC_FPGA_CONF_RST_BIT;
    return 0;
}



