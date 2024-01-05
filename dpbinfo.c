#if 0
MIT License

Copyright (c) 2024 Bernd Boeckmann

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
#endif


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>

#pragma pack(1)
typedef struct {
    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t  num_fats;
    uint16_t rootdir_entries;
    uint16_t total_sectors;
    uint8_t  media_id;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t heads;
    uint32_t hidden_sectors;
    uint32_t big_total_sectors;
    uint32_t big_sectors_per_fat;
    uint16_t flags;
    uint16_t fs_version;
    uint32_t rootdir_cluster;
    uint16_t fsinfo_sector;
    uint16_t backup_boot_sector;
    uint16_t _reserved1[6];
} bpb_t;

typedef struct {
    uint8_t  drive;
    uint8_t  unit;
    uint16_t bytes_per_sector;
    uint8_t  cluster_mask;
    uint8_t  cluster_shift;
    uint16_t reserved_sectors;
    uint8_t  num_fats;
    uint16_t rootdir_entries;
    uint16_t first_data_sector;
    uint16_t highest_cluster;
    uint16_t sectors_per_fat;
    uint16_t first_rootdir_sector;
    uint16_t device_driver_off;
    uint16_t device_driver_seg;
    uint8_t  media_id;
    uint8_t  dpb_flags;
    uint16_t next_dpb_off;
    uint16_t next_dpb_seg;
    uint16_t search_cluster;
    union {
        struct {
            uint16_t free_clusters;
        } std;
        struct {
            uint32_t free_clusters;
            uint16_t fat_mirroring;
            uint16_t fsinfo_sector;
            uint16_t backup_boot_sector;
            uint32_t first_data_sector;
            uint32_t highest_cluster;
            uint32_t sectors_per_fat;
            uint32_t rootdir_cluster;
            uint32_t search_cluster;
        } ext;
    } x;
} dpb_t;
#pragma pack

void dump_dpb( const dpb_t *dpb, int extended )
{
    if ( !dpb ) {
        printf( "DPB null pointer\n" );
        return;
    }
    printf( "drive                     %8x %11u\n",
            dpb->drive, dpb->drive );
    printf( "unit                      %8x %11u\n",
            dpb->unit, dpb->unit );
    printf( "bytes per sector          %8x %11u\n",
            dpb->bytes_per_sector, dpb->bytes_per_sector );
    printf( "cluster mask              %8x %11u\n",
            dpb->cluster_mask, dpb->cluster_mask );
    printf( "cluster shift             %8x %11u\n",
            dpb->cluster_shift, dpb->cluster_shift );
    printf( "reserved sectors          %8x %11u\n",
            dpb->reserved_sectors, dpb->reserved_sectors );
    printf( "number of FATs            %8x %11u\n",
            dpb->num_fats, dpb->num_fats );
    printf( "root directory entries    %8x %11u\n",
            dpb->rootdir_entries, dpb->rootdir_entries );
    printf( "first data sector         %8x %11u\n",
            dpb->first_data_sector, dpb->first_data_sector );
    printf( "highest cluster           %8x %11u\n",
            dpb->highest_cluster, dpb->highest_cluster );
    printf( "sectors per fat           %8x %11u\n",
            dpb->sectors_per_fat, dpb->sectors_per_fat );
    printf( "first root dir sector     %8x %11u\n",
            dpb->first_rootdir_sector, dpb->first_rootdir_sector );
    printf( "device driver header     %04x:%04x\n",
            dpb->device_driver_seg, dpb->device_driver_off );
    printf( "media id                  %8x %11u\n",
            dpb->media_id, dpb->media_id );
    printf( "dpb flags                 %8x %11u\n",
            dpb->dpb_flags, dpb->dpb_flags );
    printf( "next dpb                 %04x:%04x\n",
            dpb->next_dpb_seg, dpb->next_dpb_off );
    printf( "next cluster              %8x %11u\n",
            dpb->search_cluster, dpb->search_cluster );
    if ( !extended ) {
        printf( "free clusters             %8x %11u\n",
            dpb->x.std.free_clusters, dpb->x.std.free_clusters );
    }
    else {
        printf( "free clusters             %8lx %11lu\n",
            dpb->x.ext.free_clusters, dpb->x.ext.free_clusters );
        printf( "active FAT / mirroring    %8x %11u\n",
            dpb->x.ext.fat_mirroring, dpb->x.ext.fat_mirroring );
        printf( "fsinfo sector             %8x %11u\n",
            dpb->x.ext.fsinfo_sector, dpb->x.ext.fsinfo_sector );
        printf( "backup boot sector        %8x %11u\n",
            dpb->x.ext.backup_boot_sector, dpb->x.ext.backup_boot_sector );
        printf( "first data sector (big)   %8lx %11lu\n",
            dpb->x.ext.first_data_sector, dpb->x.ext.first_data_sector );
        printf( "highest cluster (big)     %8lx %11lu\n",
            dpb->x.ext.highest_cluster, dpb->x.ext.highest_cluster );
        printf( "sectors per fat (big)     %8lx %11lu\n",
            dpb->x.ext.sectors_per_fat, dpb->x.ext.sectors_per_fat );
        printf( "root dir start cluster    %8lx %11lu\n",
            dpb->x.ext.rootdir_cluster, dpb->x.ext.rootdir_cluster );
        printf( "next cluster (big)        %8lx %11lu\n",
            dpb->x.ext.search_cluster, dpb->x.ext.search_cluster );
    }
}

void dump_bpb( const bpb_t *bpb, int extended )
{
    if ( !bpb ) {
        printf( "BPB null pointer\n" );
        return;
    }
    printf( "bytes per sector          %8x %11u\n",
            bpb->bytes_per_sector, bpb->bytes_per_sector );
    printf( "sectors per cluster       %8x %11u\n",
            bpb->sectors_per_cluster, bpb->sectors_per_cluster );
    printf( "reserved sectors          %8x %11u\n",
            bpb->reserved_sectors, bpb->reserved_sectors );
    printf( "number of FATs            %8x %11u\n",
            bpb->num_fats, bpb->num_fats );
    printf( "root directory entries    %8x %11u\n",
            bpb->rootdir_entries, bpb->rootdir_entries );
    printf( "total sectors             %8x %11u\n",
            bpb->total_sectors, bpb->total_sectors );
    printf( "media id                  %8x %11u\n",
            bpb->media_id, bpb->media_id );
    printf( "sectors per FAT           %8x %11u\n",
            bpb->sectors_per_fat, bpb->sectors_per_fat );
    printf( "sectors per track         %8x %11u\n",
            bpb->sectors_per_track, bpb->sectors_per_track );
    printf( "number of heads           %8x %11u\n",
            bpb->heads, bpb->heads );
    printf( "hidden sectors            %8lx %11lu\n",
            bpb->hidden_sectors, bpb->hidden_sectors );
    printf( "total sectors (big)       %8lx %11lu\n",
            bpb->big_total_sectors, bpb->big_total_sectors );

    if ( extended ) {
        printf( "sectors per FAT (big)     %8lx %11lu\n",
            bpb->big_sectors_per_fat, bpb->big_sectors_per_fat );
        printf( "extended flags            %8x %11u\n",
            bpb->flags, bpb->flags );
        printf( "filesystem version        %8x %11u\n",
            bpb->fs_version, bpb->fs_version );
        printf( "root dir start cluster    %8lx %11lu\n",
            bpb->rootdir_cluster, bpb->rootdir_cluster );
        printf( "fsinfo sector             %8x %11u\n",
            bpb->fsinfo_sector, bpb->fsinfo_sector );
        printf( "backup boot sector        %8x %11u\n",
            bpb->backup_boot_sector, bpb->backup_boot_sector );
    }
}

int get_dpb_func32( uint8_t drive, dpb_t *dpb )
{
    union REGS regs;
    struct SREGS sregs;

    regs.h.ah = 0x32;
    regs.h.dl = drive;
    intdosx( &regs, &regs, &sregs );

    if ( !regs.h.al ) {
        _fmemcpy(dpb, MK_FP( sregs.ds, regs.w.bx ), 33);
    }

    return regs.h.al;
}


int get_dpb_func7302( uint8_t drive, dpb_t *dpb )
{
    uint8_t buf[0x3f];
    
    union REGS regs;
    struct SREGS sregs;

    regs.w.ax = 0x7302;
    regs.h.dl = drive;
    regs.w.si = 0xf1a6;
    regs.w.cx = sizeof(buf);
    regs.w.di = FP_OFF(buf);
    sregs.es = FP_SEG(buf);
    intdosx( &regs, &regs, &sregs );

    if ( !regs.h.al ) {
        _fmemcpy(dpb, buf+2, 0x3d);
    }

    return regs.h.al;
}


int get_bpb( uint8_t drive, int ext, int default_bpb, bpb_t *bpb )
{
    uint8_t buf[7 + sizeof(bpb_t) + 32];
    union REGS regs;
    struct SREGS sregs;

    buf[0] = 4 | ((default_bpb) ? 0 : 1);
    regs.w.ax = 0x440d;
    regs.h.bl = drive;
    regs.w.cx = (ext) ? 0x4860 : 0x0860;
    regs.w.dx = FP_OFF(buf);
    sregs.ds = FP_SEG(buf);
    intdosx( &regs, &regs, &sregs );
    if ( !regs.h.al ) {
        memcpy( bpb, buf+7, sizeof(bpb_t) );
    }
    return regs.h.al;
}


enum {
    SHOW_NONE = 0,
    SHOW_BPB,
    SHOW_DEFAULT_BPB,
    SHOW_DPB
};


int show_bpb( int drive, int ext, int def )
{
    bpb_t bpb;
    int result;

    printf( "Getting %s%sBPB for ", (ext) ? "extended " : "",
            (def) ? "default " : "" );
    if ( drive ) {
        printf( "drive %c: ", drive + 'A' - 1 );
    }
    else {
        printf( "default drive " );
    }
    printf( "via INT21h function 440Dh\n" );

    result = get_bpb( drive, ext, def, &bpb );

    if ( !result ) {
        printf( "BIOS parameter block:\n" );
        dump_bpb( &bpb, ext );
    }
    else {
        printf( "DOS error %02xh (%d)\n", result, result );
    }
    return result;
}


int show_dpb( int drive, int ext )
{
    dpb_t dpb;
    int result;

    printf( "Getting %sDPB for ", (ext) ? "extended " : "" );
    if ( drive ) {
        printf( "drive %c: ", drive + 'A' - 1 );
    }
    else {
        printf( "default drive " );
    }
    printf( "via INT 21h function %sh.\n", (ext) ? "7302" : "32" );

    if ( ext ) {
        result = get_dpb_func7302( drive, &dpb );
    }
    else {
        result = get_dpb_func32( drive, &dpb );
    }

    if ( !result ) {
        printf( "\ndevice parameter block:\n" );
        dump_dpb( &dpb, ext );
    }
    else {
        printf( "DOS error %02xh (%d)\n", result, result );
    }
    return result;
}

int main( int argc, char *argv[] )
{
    bpb_t bpb;

    int action = SHOW_NONE;
    int result = EXIT_SUCCESS;
    int ext = 0;
    uint8_t drive = 0;
    char **arg = argv + 1;

    while ( *arg ) {
        if ( isalpha( **arg ) && *(*arg+1) == ':' ) {
            drive = toupper( **arg ) - 'A' + 1;
        }
        
        else if ( !strcmp( *arg, "/bpb" ) ) action = SHOW_BPB;
        else if ( !strcmp( *arg, "/defbpb" ) ) action = SHOW_DEFAULT_BPB;
        else if ( !strcmp( *arg, "/dpb" ) ) action = SHOW_DPB;
        else if ( !strcmp( *arg, "/ext" ) ) ext = 1;
        
        arg++;
    }

    switch ( action ) {
        case SHOW_DPB:
            result = show_dpb( drive, ext );
            break;
        case SHOW_BPB:
            result = show_bpb( drive, ext, 0 );
            break;
        case SHOW_DEFAULT_BPB:
            result = show_bpb( drive, ext, 1 );
            break;
        default:
            printf( "Usage: DPBINFO /[def]bpb | /dpb [/ext] [X:]\n" );
    }

    return result;
}

