/* common shared memory */
#define SHMEM_UPPER_ADDR		(0x00000000)
#define SHMEM_UPPER_SIZE		(0x00000000)
#define SHMEM_TOP_ADDR_VBD		(0x40080000)

#define SHMEM_IPCU_BUFFER_ADDR_OFFSET			0x08
#define SHMEM_IPCU_BUFFER_SIZE_OFFSET			0x10
#define SHMEM_IPCU_SYNC_ADDR_OFFSET			0x18
#define SHMEM_IPCU_SYNC_SIZE_OFFSET			0x20
#define SHMEM_TS_READ_POINTER_ADDR_OFFSET		0x28
#define SHMEM_TS_READ_POINTER_SIZE_OFFSET		0x30
#define SHMEM_TS_WRITE_POINTER_ADDR_OFFSET		0x38
#define SHMEM_TS_WRITE_POINTER_SIZE_OFFSET		0x40
#define SHMEM_MOVIE_RECORD0_DFS_ADDR_OFFSET		0x48
#define SHMEM_MOVIE_RECORD0_DFS_SIZE_OFFSET		0x50
#define SHMEM_TERMINAL_IO_ADDR_OFFSET			0x58
#define SHMEM_TERMINAL_IO_SIZE_OFFSET			0x60

/* VBD */
#define GET_IPCU_BUFFER_ADDR_VBD	(SHMEM_TOP_ADDR_VBD + SHMEM_IPCU_BUFFER_ADDR_OFFSET)
#define GET_IPCU_BUFFER_SIZE_VBD	(SHMEM_TOP_ADDR_VBD + SHMEM_IPCU_BUFFER_SIZE_OFFSET)
#define GET_IPCU_SYNC_ADDR_VBD		(SHMEM_TOP_ADDR_VBD + SHMEM_IPCU_SYNC_ADDR_OFFSET)
#define GET_IPCU_SYNC_SIZE_VBD		(SHMEM_TOP_ADDR_VBD + SHMEM_IPCU_SYNC_SIZE_OFFSET)
#define GET_TS_READ_POINTER_ADDR_VBD	(SHMEM_TOP_ADDR_VBD + SHMEM_TS_READ_POINTER_ADDR_OFFSET)
#define GET_TS_READ_POINTER_SIZE_VBD	(SHMEM_TOP_ADDR_VBD + SHMEM_TS_READ_POINTER_SIZE_OFFSET)
#define GET_TS_WRITE_POINTER_ADDR_VBD	(SHMEM_TOP_ADDR_VBD + SHMEM_TS_WRITE_POINTER_ADDR_OFFSET)
#define GET_TS_WRITE_POINTER_SIZE_VBD	(SHMEM_TOP_ADDR_VBD + SHMEM_TS_WRITE_POINTER_SIZE_OFFSET)
#define GET_MOVIE_RECORD0_DFS_ADDR_VBD	(SHMEM_TOP_ADDR_VBD + SHMEM_MOVIE_RECORD0_DFS_ADDR_OFFSET)
#define GET_MOVIE_RECORD0_DFS_SIZE_VBD	(SHMEM_TOP_ADDR_VBD + SHMEM_MOVIE_RECORD0_DFS_SIZE_OFFSET)
#define GET_TERMINAL_IO_ADDR_VBD	(SHMEM_TOP_ADDR_VBD + SHMEM_TERMINAL_IO_ADDR_OFFSET)
#define GET_TERMINAL_IO_SIZE_VBD	(SHMEM_TOP_ADDR_VBD + SHMEM_TERMINAL_IO_SIZE_OFFSET)
