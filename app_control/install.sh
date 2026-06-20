echo "===== 任务26 APP网页控制环境一键安装 ====="
sudo apt update
# 安装python3、pip、开发依赖
sudo apt install -y python3 python3-pip python3-dev vim
# 安装网页框架bottle、GPIO库
pip3 install bottle RPi.GPIO
echo "环境安装完成！进入app_control目录执行 python3 web_server.py 启动服务"
echo "访问地址：树莓派IP:8080"