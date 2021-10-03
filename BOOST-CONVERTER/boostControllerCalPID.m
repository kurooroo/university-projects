clear all
%%
% Tham so bo bien doi boost
rC=70e-3; %esr
rL=0;
C = 47e-6; %tu dien
L = 120e-6; %cuon cam
R = 15; %Tai thuan tro
Vo = 30; %gia tri xac lap dien ap tren tu
Vg = 12; %gia tri xac lap dien ap dau vao
D = 1-Vg/Vo; %He so dieu che
IL=Vo/((1-D)*R);%gia tri xac lap dong qua cuon cam
%ham truyen giua dien ap dau ra va he so dieu che
w_esr=1/(rC*C);
w_RHP=R*(1-D)*(1-D)/L;
Q=(1-D)*R*sqrt(C/L);
w0=(1-D)/sqrt(L*C);
Gvdo=Vg/((1-D)*(1-D));
num=Gvdo*[-1/(w_esr*w_RHP) (1/w_esr)-(1/w_RHP) 1];
den=[1/(w0*w0) 1/(Q*w0) 1];
Gvd=tf(num,den);
%ham truyen bo bu
fc=1000; %tan so cat 1,5kHz
PM=50; %Du tru pha 55 degree
[mag1,phase1]=bode(Gvd,2*pi*fc);
theta=PM-(phase1+180);%tinh pha bo bu Lead - Lag
fz=fc*sqrt((1-sin(theta*pi/180))/(1+sin(theta*pi/180)));
fp=fc*sqrt((1+sin(theta*pi/180))/(1-sin(theta*pi/180)));
fl=fc/20;
numc=[1/(2*pi*fz) 1];
denc=[1/(2*pi*fp) 1];
Gc1=tf(numc,denc)*tf([1 2*pi*fl],[1 0]);
[mag2,phase2]=bode(Gc1,2*pi*fc);
kc=1/(mag1*mag2);
Gc=kc*Gc1;