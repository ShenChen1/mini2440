#!/bin/bash

#-r, -d, --root=DIR   root filesystem source code tree
rootfs_dir=rootfs

#-m, minimum I/O unit size, it's 2K(the Page size) on K9F2G08, refer to "UBI: smallest flash I/O unit:    2048" 
page_size_in_bytes=2048
#echo "Page size [$page_size_in_bytes] bytes."


#It's 64 pages per block on K9F2G08
pages_per_block=64
block_size_in_bytes=`expr $page_size_in_bytes \* $pages_per_block`
#echo "[$pages_per_block] pages per block and [$block_size_in_bytes] bytes"


#It's 2048 blocks on K9F2G08
blocks_per_device=2048
#echo "Blocks per device  [$blocks_per_device]"


#-e, logical erase block size, fixed on K9F2G08, refer to u-boot information "UBI: logical eraseblock size:  129024 bytes"
# logical erase block size is physical erase block size minus 1 page for UBI
logical_pages_per_block=`expr $pages_per_block - 1`
logical_erase_block_size=`expr $page_size_in_bytes \* $logical_pages_per_block`
#echo "Logical erase block size:  [$logical_erase_block_size] bytes."


# wear_level_reserved_blocks is 1% of total blcoks per device
wear_level_reserved_blocks=`expr $blocks_per_device / 100`
#echo "Reserved blocks for wear level [$wear_level_reserved_blocks]"


#The rootfs partition size in bytes
partition_sizeM=20
partition_size_in_bytes=`expr $partition_sizeM \* 1024 \* 1024`
partition_physical_blocks=`expr $partition_size_in_bytes / $block_size_in_bytes`
#echo "Partition size [$partition_size_in_bytes] bytes and [$partition_physical_blocks] blocks."


#Logical blocks on a partition = physical blocks on a partitiion - reserved for wear level 
patition_logical_blocks=`expr $partition_physical_blocks - $wear_level_reserved_blocks`
#echo "Logical blocks in a partition [$patition_logical_blocks]"


#File-system volume = Logical blocks in a partition * Logical erase block size
fs_vol_size=`expr $patition_logical_blocks \* $logical_erase_block_size`
#echo "File-system volume [$fs_vol_size] bytes."


config_file=ubinize.cfg
image_name=ubifs.img

echo "Generating $image_name file by mkfs.ubifs..."
mkfs.ubifs -x lzo -m $page_size_in_bytes -e $logical_erase_block_size -c $patition_logical_blocks -r $rootfs_dir -o $image_name

echo "Generating configuration file..."
echo "[ubifs-volume]" > $config_file
echo "mode=ubi" >> $config_file
echo "image=$image_name" >> $config_file
echo "vol_id=0" >> $config_file
echo "vol_size=$fs_vol_size" >> $config_file
echo "vol_type=dynamic" >> $config_file
echo "vol_name=rootfs" >> $config_file
echo "vol_flags=autoresize" >> $config_file
echo "vol_alignment=1" >> $config_file

#Default setting by UBIFS
sub_page_size=512
vid_head_offset=512

echo "Generating ubi.img file by ubinize..."
ubinize $config_file -o ubi.img -m $page_size_in_bytes -p $block_size_in_bytes -s $sub_page_size -O $vid_head_offset
