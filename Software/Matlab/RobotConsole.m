%---------------------------------------------------------------%
% RobotConsole.m
% Control and monitoring console for final project robot.
% RBE-2001 B17 Team 10
%---------------------------------------------------------------%

% Bluetooth parameters
clear vars; clc
BLUETOOTH_NAME = 'Arduino';
BLUETOOTH_CHANNEL = 1;
BYTE_BEGIN = hex2dec('01');
BYTE_UPDATE = hex2dec('03');

% Shortcuts
NL = newline;
TAB = char(9);

%---------------------------------------------------------------%

% Program Initialization
disp(['RobotConsole' NL])

% Initialize Bluetooth
if ~exist('bluetooth', 'var')
    disp(['Finding bluetooth module "' BLUETOOTH_NAME '" ...'])
    bluetooth = Bluetooth('Arduino', 1);
    
end
if ~exist('serial', 'var')
    serial = ArduinoSerial(bluetooth);
end

% Connect to Bluetooth
disp(['Connecting to "' BLUETOOTH_NAME '" ...'])
if serial.open()
    disp('Connected!')
    serial.writeByte(BYTE_BEGIN);
else
    disp('Connection failed!')
    return
end

%---------------------------------------------------------------%

% Communication loop
while 1
    % Get heading
    serial.writeByte(BYTE_UPDATE);
    serial.wait(24);
    t = serial.readFloat();
    h = serial.readFloat();
    dF = serial.readFloat();
    dB = serial.readFloat();
    dL = serial.readFloat();
    dR = serial.readFloat();
    
    % Display Robot Statuses
    clc
    disp(['RobotConsole' NL])
    disp(['Time: ' num2str(t, '%.2f') 's'])
    disp(['Heading: ' num2str(h, '%.3f') 'rad'])
    disp('Sonar: ')
    disp([TAB 'F: ' num2str(dF, '%.2f') 'm'])
    disp([TAB 'B: ' num2str(dB, '%.2f') 'm'])
    disp([TAB 'L: ' num2str(dL, '%.2f') 'm'])
    disp([TAB 'R: ' num2str(dR, '%.2f') 'm'])
    
    % Graph Heading
    x = sin(h);
    y = cos(h);
    figure(1)
    hold off
    plot([0 x], [0 y], 'color', 'b', 'linewidth', 2)
    hold on
    text(1.05*x, 1.05*y, 'Heading')
    title('Robot Console')
    xlabel('Field x')
    ylabel('Field y')
    axis equal
    axis([-1.1, 1.1, -1.1, 1.1])
    grid on
    drawnow
end