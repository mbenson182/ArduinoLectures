% ModeExamples.m
% Creates a few plots to demonstrate how different pin modes work for 
% Arduino external interrupts.

close all


t = 0:0.1:3; %our time steps
input = 5*(randi(2,[1,length(t)]) - 1); %randonly generate input signal flipping between 0 and 1
input = input + (0.1*randn([1,length(t)])); %add some random noise

highThreshold = 3.0; %threshold for HIGH values

inputAxes = [t(1) t(end) -1 6];
interruptAxes = [t(1) t(end) -0.2 1.2];

% Plot input signal
%{
plot(t,input,'bd-.',[t(1) t(end)], [highThreshold highThreshold],'r:')
title('Input Signal')
xlabel('Time (s)')
ylabel('Value on Pin 2')
legend('Pin 2','Threshold','LOCATION','EAST')
%}

%% LOW Mode
%  In LOW mode, the interrupt function is called whenever the pin is LOW

lowMode = zeros(1,length(t));
for i = 1:length(t)
  if input(i) <= highThreshold
    lowMode(i) = 1; 
  end
end


figure()
plot(t,input,'b.-.',[t(1) t(end)], [highThreshold highThreshold],'m:')
title('Interrupt Signal: LOW Mode')
xlabel('Time (s)')
ylabel('Value on Pin 2')
axis(inputAxes)
hold on

for i = 1:length(t)
  if lowMode(i) == 1
    plot(t(i),input(i),'rd')
  end
end
legend('Input','High Thresh','Interrupt')

%% FALLING Mode
%  In FALLING mode, the interrupt function is called whenever the pin goes
%  from HIGH to LOW

fallingMode = zeros(1,length(t));
for i = 1:length(t)
  
  %Because this mode requires knowledge of prior value, skip for first time
  %step
  if i == 1
    fallingMode(i) = 0; 
    continue;
  end
  
  if input(i) <= highThreshold && input(i-1) > highThreshold
    fallingMode(i) = 1; 
  end
end

figure()
plot(t,input,'b.-.',[t(1) t(end)], [highThreshold highThreshold],'m:')
title('Interrupt Signal: FALLING Mode')
xlabel('Time (s)')
ylabel('Value on Pin 2')
axis(inputAxes)
hold on

for i = 1:length(t)
  if fallingMode(i) == 1
    plot(t(i),input(i),'rd')
  end
end
legend('Input','High Thresh','Interrupt')

%% CHANGE Mode
%  In CHANGE mode, the interrupt function is called whenever the pin
%  changes between LOW or HIGH

changeMode = zeros(1,length(t));
for i = 1:length(t)
  
  %Because this mode requires knowledge of prior value, skip for first time
  %step
  if i == 1
    changeMode(i) = 0; 
    continue;
  end
  
  if ((input(i) <= highThreshold && input(i-1) > highThreshold) || ...
    (input(i) > highThreshold && input(i-1) <= highThreshold))
    changeMode(i) = 1; 
  end
end

figure()
plot(t,input,'b.-.',[t(1) t(end)], [highThreshold highThreshold],'m:')
title('Interrupt Signal: CHANGE Mode')
xlabel('Time (s)')
ylabel('Value on Pin 2')
axis(inputAxes)
hold on

for i = 1:length(t)
  if changeMode(i) == 1
    plot(t(i),input(i),'rd')
  end
end
legend('Input','High Thresh','Interrupt')