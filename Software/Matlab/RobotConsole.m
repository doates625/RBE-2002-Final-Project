%---------------------------------------------------------------%
% RobotConsole.m
% Control and monitoring console for final project robot.
% RBE-2001 B17 Team 10
%---------------------------------------------------------------%

%% Clear Workspace
close all
clear vars
clc

%% Constants
BLUETOOTH_NAME = 'Arduino';
BLUETOOTH_CHANNEL = 1;
TELEOP_VOLTAGE = 6;

%% User Interface Initialization
displayTitle();
try
    xbox = XboxController(1, 0.1, 1.5);
catch
    disp('Xbox controller not connected!')
    return
end
ui = RobotUI();
com = RobotComms(BLUETOOTH_NAME, BLUETOOTH_CHANNEL);

%% Connect to Robot
while 1
    displayTitle();
    
    if ui.connectPressed()
        disp('Connecting to robot...')
        [s, msg] = com.connect();
        disp(msg)
        pause(1)
        if s ~= 1
            com.disconnect();
            return
        else
            break
        end
    else
        disp('Press ''Connect'' to connect to robot.')
    end

    ui.update();
end

%% Wait for Begin Button
while 1
    displayTitle();
    
    if ui.beginPressed()
        disp('Beginning teleop')
        pause(1)
        break
    else
        disp('Press ''Begin'' to begin teleop.')
    end
    
    ui.update();
end

%% Teleoperated Loop
while 1
    displayTitle();
    disp('Teleoperated Mode')
    
    [vL, vR] = getDriveVoltage(xbox, TELEOP_VOLTAGE);
    if ~com.setDriveVoltage(vL, vR)
        disp('Teleop response timeout!')
        com.disconnect();
        return
    end
    
    if ui.disconnectPressed()
        com.disconnect();
        disp('Disconnect by user.')
        return
    end
    
    ui.update();
end

%% Ignore remaining code for now
return

while 1
    displayTitle();
    
    switch state
        % Active robot control
        case 'Active'
            
            % Disconnect command
            if ui.disconnectPressed()
                com.disconnect();
                disp('Disconnected from robot.')
                pause(1)
                return
            end
            
            % Get Robot Data
            rd = com.getData();

            % Display Data
            displayTitle();
            disp(['Time: ' num2str(rd.time, '%.2f') 's'])
            disp(['Heading: ' num2str(rd.heading, '%.3f') 'rad'])
            disp('Sonar: ')
            disp(['    F: ' num2str(rd.sonar.f, '%.2f') 'm'])
            disp(['    B: ' num2str(rd.sonar.b, '%.2f') 'm'])
            disp(['    L: ' num2str(rd.sonar.l, '%.2f') 'm'])
            disp(['    R: ' num2str(rd.sonar.r, '%.2f') 'm'])

            % Graph Data
            ui.update(rd);  
    end
end

%% Helper Functions

% Clears command line and displays program title
function displayTitle()
    clc
    disp(['Robot Console' newline])
end

% Gets drive voltages from Xbox controller
function [vL, vR] = getDriveVoltage(xbox, vMax)
    [x, y] = xbox.LJS();
    k = vMax * sqrt(2) * 0.5;
    vL = k*(y + x);
    vR = k*(y - x);
end