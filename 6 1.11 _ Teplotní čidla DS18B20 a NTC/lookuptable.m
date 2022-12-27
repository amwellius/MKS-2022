close all; clear all;

q = csvread("ntc.csv");

x = q(:,1);
y = q(:,2);

t = x;
ad = 1024 .* (y ./ (y + 10));

figure;
plot(ad,t, 'blue', 'LineWidth', 3)
ylim([-30 150]);

p = polyfit(ad, t, 10);
ad2 = 0:1023;
t2 = round(polyval(p, ad2), 1);
hold on
plot(ad2, t2, 'r')

dlmwrite('data.dlm', t2 * 10, ',');