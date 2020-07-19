clc
clear
x=importdata('1.txt')';%如果是10进制就按照这条语句
% a=textread('1.txt','%s')';%如果是16进制就取消掉该句注释，并且将上一条语句注释掉
% x=bitcmp(uint8(hex2dec(a)))' %16进制转化为10进制数，存入alpha矩阵 
L = length(x)   %序列长度
Fs = 1000000;  %采样率
t=(0:L-1);
f=(0:L-1)*Fs/L;%DFT转换后频率的坐标值
subplot(2,1,1);
plot(t,x)
xlabel('点序列')
ylabel('幅度')
y=fft(x,L);
mag=abs(y)
subplot(2,1,2);
plot(f,mag)%画出频率和模的关系
xlabel('频率（hz）')
ylabel('幅度')


% 从excel中得到数据
x = xlsread('cccs.xlsx');
% 数据转置
x = x';
X1=x(1,:);
X3=x(3,:);
% 画出原始数据
plot(x(1,:),x(3,:),'r');
% 多项式拟合，如果拟合效果不好，可以得高多项式的阶数，也就是下面最后一个参数，多项式越高，计算量就越大， 
% P是多项式的表达式
[p,s]=polyfit(x(1,:),x(3,:),1);  % 15是多项式阶数
% 用上面拟合出的多项式计算对应的结果及与原始数据的偏差
[y_fit,DELTA]=polyval(p,x(1,:),s);
% 画出拟合后的结果
hold on 
plot(x(1,:),y_fit,'b');
% 给曲线加标注
legend('原始曲线','拟合后曲线');
% 显示多项式各阶系数
p

