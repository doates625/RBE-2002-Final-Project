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
BYTE_RESET = hex2dec('02');
BYTE_UPDATE = hex2dec('03');

% Shortcuts
NL = newline;

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

% Initialize Xbox controller
xbox = XboxController(1, 0.07, 2);

% Connect to Bluetooth
disp(['Connecting to "' BLUETOOTH_NAME '" ...'])
if serial.open()
    disp('Connected!')
    disp('Press ''Start'' to begin')
    disp('Press ''Back'' to end program')
    switch xbox.waitForInput({'Start', 'Back'})
        case 'Start'
            serial.writeByte(BYTE_START);
        case 'Back'
            serial.writeByte(BYTE_RESET);
            serial.close();
    end
else
    disp('Connection failed!')
    return
end

%---------------------------------------------------------------%

% Communication loop
while 1
    
    % Back button resets Arduino and ends program
    if xbox.Back()
        serial.writeByte(BYTE_RESET);
        serial.close();
        disp('Program terminated.')
        pause(1)
        break
    end
    
    % Get heading
    serial.writeByte(BYTE_UPDATE);
    serial.wait(4);
    heading = serial.readFloat();
    
    % Display heading
    clc
    disp(['RobotConsole' NL])
    disp(['Heading: ' num2str(heading)])
end