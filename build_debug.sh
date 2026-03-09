make realclean
make PLAT=fvp all fip BL33=./myuefi/ARMUEFI-m15.fd DEBUG=1

cp build/fvp/debug/bl1.bin output/
cp build/fvp/debug/bl2.bin output/
cp build/fvp/debug/bl31.bin output/

cp build/fvp/debug/bl1/bl1.dump output/
cp build/fvp/debug/bl2/bl2.dump output/
cp build/fvp/debug/bl31/bl31.dump output/

# 定义时间格式：mmddtttt（mm=月份，dd=日期，tttt=时+分，24小时制）
TIME_SUFFIX=$(date +%m%d%H%M)

# 定义源文件路径和目标文件夹
OUTPUT_DIR="output"
BL1_SRC="build/fvp/debug/bl1.bin"
# BL2_SRC="build/fvp/debug/bl2.bin"
# BL31_SRC="build/fvp/debug/bl31.bin"
FIP_SRC="build/fvp/debug/fip.bin"
# 确保output文件夹存在
mkdir -p ${OUTPUT_DIR}

# 复制并按时间重命名bin文件
cp ${BL1_SRC} ${OUTPUT_DIR}/bl1-${TIME_SUFFIX}.bin
# cp ${BL2_SRC} ${OUTPUT_DIR}/bl2-${TIME_SUFFIX}.bin
# cp ${BL31_SRC} ${OUTPUT_DIR}/bl31-${TIME_SUFFIX}.bin
cp ${FIP_SRC} ${OUTPUT_DIR}/fip-${TIME_SUFFIX}.bin


# 复制dump文件（保持原命名，若需要重命名可参考bin文件的方式）
cp build/fvp/debug/bl1/bl1.dump ${OUTPUT_DIR}/
cp build/fvp/debug/bl2/bl2.dump ${OUTPUT_DIR}/
cp build/fvp/debug/bl31/bl31.dump ${OUTPUT_DIR}/
