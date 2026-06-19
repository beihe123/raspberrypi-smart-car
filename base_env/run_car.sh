# 任务6：在智能小车上编译、运行C语言程序一键执行脚本
# 知识点：VIM编辑器、GCC编译器、C程序编译/链接/运行、程序启停管理
# 使用命令：bash base_env/run_car.sh [参数]

# 定义项目工作目录（对应任务5创建的 /home/pi/code 开发目录）
WORK_DIR="/home/pi/code/raspberrypi-smart-car"
# C语言源码存放目录
SRC_DIR="$WORK_DIR/c_src"

# ===================== 工具安装函数 =====================
# 1. 安装VIM编辑器、GCC编译器（任务6前置依赖）
install_env() {
    echo "===== 开始安装任务6所需开发工具:vim + gcc编译器 ====="
    sudo apt update
    # 安装vim编辑器
    sudo apt-get install -y vim
    # 安装gcc C语言编译器
    sudo apt-get install -y gcc
    echo "工具安装完成,可使用vim编辑代码、gcc编译C程序"
}

# ===================== C程序编译函数 =====================
# 一键完整编译C文件，生成可执行程序
build_c() {
    local src_name=$1
    # 判断是否传入源码文件名
    if [ -z "$src_name" ]; then
        echo "错误:请传入C源码文件名,示例:bash base_env/run_car.sh build HelloWorld.c"
        return 1
    fi
    local src_file="$SRC_DIR/$src_name"
    # 提取文件名（去掉.c后缀作为可执行程序名）
    local exe_name=${src_name%.c}
    local exe_file="$SRC_DIR/$exe_name"

    # 判断源码文件是否存在
    if [ ! -f $src_file ]; then
        echo "错误:源码文件 $src_file 不存在，请检查文件路径！"
        return 1
    fi

    echo "===== GCC编译开始:$src_name ====="
    # gcc一键编译链接，对应任务6编译命令 gcc HelloWorld.c -o Hello
    gcc $src_file -o $exe_name
    # 判断编译是否成功
    if [ $? -eq 0 ]; then
        echo "编译成功！生成可执行文件：$exe_name"
        echo "文件列表："
        ls -l $SRC_DIR
    else
        echo "编译失败,请检查C语言代码语法!"
    fi
}

# ===================== 运行C程序函数 =====================
run_c() {
    local exe_name=$1
    if [ -z "$exe_name" ]; then
        echo "错误:请传入可执行文件名,示例:bash base_env/run_car.sh run Hello"
        return 1
    fi
    local exe_file="$SRC_DIR/$exe_name"
    if [ ! -f $exe_file ]; then
        echo "错误:可执行文件 $exe_file 不存在，请先编译！"
        return 1
    fi
    echo "===== 开始运行程序 ./$exe_name ====="
    cd $SRC_DIR
    ./$exe_name
}

# ===================== 新建HelloWorld示例代码（任务6演示用） =====================
create_demo() {
    echo "===== 自动生成任务6示例 HelloWorld.c 代码 ====="
    mkdir -p $SRC_DIR
    cat > $SRC_DIR/HelloWorld.c << EOF
#include <stdio.h>
int main(void)
{
    printf("Hello World!\n");
    return 0;
}
EOF
    echo "示例代码创建完成，路径：$SRC_DIR/HelloWorld.c"
    echo "可执行vim HelloWorld.c编辑代码"
}

# ===================== 程序清理函数 =====================
clean() {
    echo "===== 清理编译生成的可执行文件 ====="
    rm -rf $SRC_DIR/*.out $SRC_DIR/*.i $SRC_DIR/*.s $SRC_DIR/*.o
    echo "清理完成"
}

# ===================== 使用帮助文档（整合任务6全部操作知识点） =====================
show_help() {
echo "==================== 任务6脚本使用帮助 ===================="
echo "前置工具安装：  bash base_env/run_car.sh install"
echo "1. 生成HelloWorld演示C代码: bash base_env/run_car.sh demo"
echo "2. 编译C源码文件(gcc):    bash base_env/run_car.sh build 文件名.c"
echo "   示例:bash base_env/run_car.sh build HelloWorld.c"
echo "3. 运行编译后的程序：        bash base_env/run_car.sh run 可执行文件名"
echo "   示例:bash base_env/run_car.sh run HelloWorld"
echo "4. 清理编译中间文件：        bash base_env/run_car.sh clean"
echo ""
echo "===== 任务6配套手动操作命令(可直接复制使用) ====="
echo "【VIM编辑器操作】"
echo "安装vim:sudo apt-get install -y vim"
echo "打开/新建文件:vim HelloWorld.c"
echo "vim切换插入模式:i / I / o / A"
echo "保存退出:ESC 输入 :wq"
echo "不保存退出:ESC 输入 :q!"
echo ""
echo "【GCC分步编译完整流程(任务6示例)】"
echo "预处理:gcc -E HelloWorld.c -o HelloWorld.i"
echo "编译:gcc -S HelloWorld.i -o HelloWorld.s"
echo "汇编:gcc -c HelloWorld.s -o HelloWorld.o"
echo "链接生成程序:gcc HelloWorld.o -o HelloWorld"
echo "一键编译:gcc HelloWorld.c -o HelloWorld"
echo "运行程序：./HelloWorld"
echo ""
echo "【FileZilla文件传输】"
echo "连接信息:树莓派IP,端口22,用户名pi,密码raspberry"
echo "=========================================================="
}

# ===================== 命令参数分发逻辑 =====================
case $1 in
    install)
        install_env
        ;;
    demo)
        create_demo
        ;;
    build)
        build_c $2
        ;;
    run)
        run_c $2
        ;;
    clean)
        clean
        ;;
    *)
        show_help
        ;;
esac


echo "===== 开始编译所有小车硬件驱动程序 ====="

# 1. 任务7 电机驱动 motor_driver.c
echo "编译电机驱动 motor_driver..."
gcc hardware_driver/motor_driver.c -o motor_driver -lwiringPi -lpthread

# 2. 任务8 状态指示灯 status_led.c
echo "编译LED状态指示 status_led..."
gcc hardware_driver/status_led.c -o status_led -lwiringPi

# 3. 任务9 蜂鸣器报警 alarm_buzzer.c
echo "编译蜂鸣器报警 alarm_buzzer..."
gcc hardware_driver/alarm_buzzer.c -o alarm_buzzer -lwiringPi

# 4. 任务10 按键输入 key_input.c
echo "编译按键检测 key_input..."
gcc hardware_driver/key_input.c -o key_input -lwiringPi

echo "===== 全部编译完成，运行示例： ====="
echo "sudo ./motor_driver    测试电机"
echo "sudo ./status_led      测试LED指示灯"
echo "sudo ./alarm_buzzer    测试蜂鸣器音乐/报警"
echo "sudo ./key_input       测试按键切换灯光"

四、使用方法
给脚本添加执行权限（终端执行一次）：
chmod +x base_env/run_car.sh

一键批量编译所有驱动:
./base_env/run_car.sh

单独运行对应程序，例如测试按键：
sudo ./key_input

# ========== 任务11、12 基础运动模块 basic_move ==========
echo "===== 正在编译基础运动控制模块 basic_move ====="
gcc basic_move/basic_move.c -o basic_move -lwiringPi -lpthread
echo "编译完成,运行测试命令sudo ./basic_move"
echo "------------------------------------------------"

# ========== 任务13、14 红外循迹模块 tracking_line ==========
echo "===== 正在编译红外循迹模块 tracking_line ====="
gcc tracking_line/tracking_line.c -o tracking_line -lwiringPi -lpthread
echo "编译完成,运行测试命令:sudo ./tracking_line"


# ========== 任务15、16 红外避障模块 ir_obstacle ==========
echo "===== 正在编译红外避障模块 ir_obstacle ====="
gcc ir_obstacle/ir_obstacle.c -o ir_obstacle -lwiringPi -lpthread
echo "编译完成，运行测试命令：sudo ./ir_obstacle"
echo "------------------------------------------------"