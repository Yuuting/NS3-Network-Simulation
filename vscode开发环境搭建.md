# ns3+VScode+gcc+gdb开发环境搭建

## 1.安装VSCODE
```
sudo snap install code
```
## 2.安装c/c++扩展
在搜索框中搜索，顺便可以安装中文简体的扩展。
## 3.配置代码智能提示功能
```
//以root身份启动vscode
sudo code --user-data-dir="~/.vscode-root"
//以root身份启动当前目录下的vscode
sudo code . --user-data-dir=/root/.vscode-root
//以普通身份运行code不需要--user-data-dir=/root/.vscode-root
```
然后，在VScode界面中，按F1调出命令面板，配置相关设置“c/c++:Edit configurations(json)”:
```json
{
    "configurations": [
        {
            "name": "Linux",
            "includePath": [
                "${workspaceFolder}/build/ns3/**",
                "${workspaceFolder}/build/**"
            ],
            "defines": [],
            "compilerPath": "/usr/bin/gcc",
            "cStandard": "gnu17",
            "cppStandard": "gnu++14",
            "intelliSenseMode": "linux-gcc-x64"
        }
    ],
    "version": 4
}
```
这样就可以进行代码提示。
## 4.配置build
菜单栏Terminal->Configure Dafault Build Task->C/C++:cpp build active file
```json
{
	"version": "2.0.0",
	"tasks": [
		{
			"type": "cppbuild",
			"label": "C/C++: cpp 生成活动文件",
			"command": "./waf",
			"args": [],
			"options": {
				"cwd": "${workspaceFolder}"
			},
			"problemMatcher": [
				"$gcc"
			],
			"group": {
				"kind": "build",
				"isDefault": true
			},
			"detail": "编译器: /usr/bin/cpp"
		},
		{
			"type": "cppbuild",
			"label": "C/C++: cpp 生成活动文件",
			"command": "./waf",
			"args": [],
			"options": {
				"cwd": "${workspaceFolder}"
			},
			"problemMatcher": [
				"$gcc"
			],
			"group": {
				"kind": "build",
				"isDefault": true
			},
			"detail": "编译器: /usr/bin/cpp"
		}
	]
}
```
## 5.配置调试
按F5，选择“C++(GDB/LLDB)”,之后选择"waf-Build and debug active file".弹出窗口选择open launch.json.
```json
{
    // 使用 IntelliSense 了解相关属性。 
    // 悬停以查看现有属性的描述。
    // 欲了解更多信息，请访问: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [
        {
            "name": "waf - 生成和调试活动文件",
            "type": "cppdbg",
            "request": "launch",
            //替换成自己的scratch目录
            "program": "/home/ating/下载/ns-allinone-3.33/ns-3.33/build/scratch/${fileBasenameNoExtension}",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            //"console": "externalTerminal",
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "为 gdb 启用整齐打印",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "C/C++: cpp 生成活动文件",
            "miDebuggerPath": "/usr/bin/gdb"
        }
    ]
}
```
提示无法找到.so文件，需要拷贝ns3.33/build/lib下所有的文件至根目录下的lib。
即：
进入ns3.33/build目录，执行
```
cp lib/* /lib
```
之后F5就可以调试程序了。
