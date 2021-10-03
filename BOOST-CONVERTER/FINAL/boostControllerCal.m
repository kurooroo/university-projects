clear all
clc
%%
% Tham so bo bien doi boost
rC=70e-3; %esr
rL=0;
C = 470e-6; %tu dien
L = 150e-6; %cuon cam
R = 15; %Tai thuan tro
Vo= 30; %gia tri xac lap dien ap tren tu
Vg= 12; %gia tri xac lap dien ap dau vao
D = 1-Vg/Vo; %He so dieu che
IL=Vo/((1-D)*R);%gia tri xac lap dong qua cuon cam
%ham truyen giua dien ap dau ra va he so dieu che
w_esr=1/(rC*C);
f_esr = w_esr/(2*pi);
w_RHP=R*(1-D)*(1-D)/L;
f_RHP = w_RHP/(2*pi);
Q=(1-D)*R*sqrt(C/L);
w0=(1-D)/sqrt(L*C);
f0 = w0/(2*pi);
Gvdo=Vg/((1-D)*(1-D));
num=Gvdo*[-1/(w_esr*w_RHP) (1/w_esr)-(1/w_RHP) 1];
den=[1/(w0*w0) 1/(Q*w0) 1];
Gvd=tf(num,den);
%ham truyen bo bu
wz1= w0; %chon bang tan so fo
wz2= w0; %chon bang tan so fo
wp1 = w_esr;
wp2 = w_RHP;
numc=[1/(wz1*wz2) (1/wz1)+(1/wz2) 1];
denc=[1/(wp1*wp2) (1/wp1)+(1/wp2) 1];
Gc1=tf(numc,denc)*tf(1,[1 0]);%ham truyen bo bu voi kc=1
fc=750; %tan so cat 1,5kHz
[mag1,phase1]=bode(Gvd,2*pi*fc);
[mag2,phase2]=bode(Gc1,2*pi*fc);
kc=1/(mag1*mag2);
Gc=kc*Gc1;