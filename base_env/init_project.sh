# 任务5：创建智能小车开发目录完整脚本
# 5.1 新建用户、配置root权限；5.2 自动生成小车全套工作目录

echo "==================== 智能小车项目环境初始化脚本 ===================="
echo "【5.1 用户账户配置步骤说明】"
echo "1. 设置root管理员密码命令:sudo passwd root"
echo "2. 切换root用户命令:su root"
echo "3. 创建专属开发用户命令:adduser caruser"
echo "====================================================================="
echo ""

# -------------------------- 5.2 创建小车工作目录 --------------------------
# 定义基础工作目录（对应任务5.2 /home/pi/code）
BASE_WORK_DIR="/home/pi/code"
# 完整项目根目录
CAR_PROJECT_DIR="${BASE_WORK_DIR}/raspberrypi-smart-car"

# 1. 进入pi家目录，创建code总工作目录
echo "步骤1：创建基础工作目录 /home/pi/code"
cd /home/pi
mkdir -p $BASE_WORK_DIR

# 2. 创建智能小车全套项目子目录（所有功能模块文件夹一次性生成）
echo "步骤2：创建小车完整项目开发目录"
mkdir -p $CAR_PROJECT_DIR/{config,base_env,hardware_driver,basic_move,tracking_line,ir_obstacle,ultrasonic,ir_remote,wifi_network,app_control,docs}

# 输出目录创建完成信息
echo ""
echo "==================== 目录创建完成 ===================="
echo "基础工作目录：$BASE_WORK_DIR"
echo "智能小车完整项目目录：$CAR_PROJECT_DIR"
echo ""
echo "项目内部目录结构："
ls $CAR_PROJECT_DIR
echo ""
echo "【用户操作补充（手动执行，脚本仅提示）】"
echo "如需新建专属开发用户,执行以下root权限命令:"
echo "  sudo passwd root        # 设置root密码"
echo "  su root                 # 切换超级管理员"
echo "  adduser caruser         # 创建小车开发专用用户caruser"
echo "======================================================"