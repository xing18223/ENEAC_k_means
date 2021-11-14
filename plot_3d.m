array = csvread("output.csv");
x = array(:, 1);
y = array(:, 2);
z = array(:, 3);
c = array(:, 4);
% plot3(x,y,z, 'o');
scatter3(x,y,z, 40, c, 'filled');
