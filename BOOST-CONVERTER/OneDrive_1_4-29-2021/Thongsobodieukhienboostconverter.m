clear all
clc
R=10;%Tai thuan tro
C=40e-6;%tu dien
L=480e-6;%cuon cam
Vg=12;%gia tri xac lap dien ap dau vao
Vo=20;%Gia tri xac lap dien ap tren tu
D=1-Vg/Vo;%He so dieu che
IL=Vo/((1-D)*R);%gia tri xac lap dong qua cuon cam
%Ham truyen giua dien ap dau ra/he so dieu che
Q=(1-D)*R*sqrt(C/L);
w_0=(1-D)/sqrt(L*C);
w_z=(1-D)*(1-D)*R/L;
Gvd0=Vo/(1-D);
num=Gvd0*[-1/(w_z) 1];
den=[1/(w_0*w_0) 1/(Q*w_0) 1];
Gvd=tf(num,den);
%ham truyen bo bu
fc=600; %tan so cat 600Hz
PM=50; %Du tru pha 50 degree
[mag1,phase1]=bode(Gvd,2*pi*fc);
theta=PM-(phase1+180);%tinh pha bo bu PID
fz=fc*sqrt((1-sin(theta*pi/180))/(1+sin(theta*pi/180)));
fp=fc*sqrt((1+sin(theta*pi/180))/(1-sin(theta*pi/180)));
fl=fc/20;
numc=[1/(2*pi*fz) 1];
denc=[1/(2*pi*fp) 1];
Gc1=tf(numc,denc)*tf([1 2*pi*fl],[1 0]);
[mag2,phase2]=bode(Gc1,2*pi*fc);
kc=1/(mag1*mag2);
Gc=kc*Gc1;
Gh=Gc*Gvd;
margin(Gc*Gvd)
x1e=Vg/((1-D)*(1-D)*R);
x2e=Vg/(1-D);
num1=[C*x2e 2*(1-D)*x1e];
den1=[L*C L/R (1-D)*(1-D)];
Gid=tf(num1,den1);