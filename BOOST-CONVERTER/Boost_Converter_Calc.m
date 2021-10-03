clear;
Vin_min     = 5;   % V
Vin         = 12;   % V
Vin_max     = 30;   % V
Vo        = 30;	% V
Po        = 60;	% W
f           = 100e3;	% Hz
Vo_ripple =0.5; %Voripple 1% = 1 V
Vo_tran = 5; % Voltage dip in transition is 20%Vo, 20 V
Itran = 5 ; % Current transition 0 - 6 A
f_bw  = 8e3; % bandwidth frequency = 8 kHz
		
Io = Po/Vo; % Output nominal current
% Calculation output capacitor by voltage ripple
Co_min_ripple = (Io/Vo_ripple)*(1-Vin_min/Vo)*(1/f); % C in F
Co_tran = (Itran/Vo_tran)*(1/(4*f_bw));

%Calculation of L
fz = 5e3; % Zero of compensator is about 11 kHz
%C = Co_tran;
C =7*Co_min_ripple;
Rmin = Vo/Io; 
Go = 20*log(0.1*Vo^2/Vin); % voltage sense divider is 0.1
Lmax = C*(0.1*Rmin*(Vin_min/Vo))^2; %L in H, M=10 (1/M=0.1)
Lmin = (1/C)*((1/(2*pi))*(Vin_max/Vo)*10^(Go/30)*(1/(2*fz)))^2;
L = 0.9*Lmax;
%Transfer function
rc = 20e-3;%ESR
rL = 50e-3; %resistance of the inductor L
wz1 = 1/(rc*C);
R = Rmin;
Gdo =0.1*Vo^2/Vin;%0,1 is voltage sense divider
wRHP_z = (R/L)*(Vin/Vo)^2;
wo = (1/sqrt(L*C))*Vin/Vo;
Q = wo/(rL/L+1/(C*(R+rc)));
fo = wo/(2*pi);
fRHP_z = wRHP_z/(2*pi);
fwz1 = wz1/(2*pi);
s = tf('s');
Gdv = Gdo*(1+s/wz1)*(1-s/wRHP_z)/(1+s/(wo*Q)+s^2/wo^2);%Open loop converter transfer function
hold on;
grid on;
h = bodeplot(Gdv);
setoptions(h,'FreqUnits','Hz');
%Compensator
wz = 2*pi*1.5e3;
wp = 2*pi*15e3;
Gs =(1+s/wz)/(1+s/wp);
Go_loop = Gdv*Gs;
bodeplot(Gs);
bodeplot(Go_loop)
