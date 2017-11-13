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
        com.startRobot();
        break
    elseif ui.disconnectPressed()
        com.disconnect();
        return
    else 
        disp('Press ''Begin'' to begin teleop.')
        disp('Press ''Disconnect'' to terminate program.')
    end
    
    ui.update();
end

%% Teleoperated Loop

% DEBUG
xMax = 0;
yMax = 0;

while 1
    displayTitle();
    disp('Teleoperated Mode')
    
    % Control drive with Xbox
    [vL, vR] = getDriveVoltage(xbox, TELEOP_VOLTAGE);
    [s, error] = com.setDriveVoltage(vL, vR);
    if s ~= 1
        disp(error)
        com.disconnect();
        return
    end
    
    % Get odometry data
    [odm, s, error] = com.getOdometryData();
    if s ~= 1
        disp(error)
        com.disconnect();
        return
    end
    
    % DEBUG
    if odm.x > xMax
        xMax = odm.x;
    end
    if odm.y > yMax
        yMax = odm.y;
    end
    
    % Display odometry data
    disp('Odometry:')
    disp(['x: ' num2str(odm.x, '%.6f') 'm'])
    disp(['y: ' num2str(odm.y, '%.6f') 'm'])
    disp(['h: ' num2str(odm.h * 180 / pi, '%.0f') 'deg'])
    disp(['xMax: ' num2str(xMax, '%.6f') 'm'])
    disp(['yMax: ' num2str(yMax, '%.6f') 'm'])
    pause(0.1)
    
    % Check disconnect button on UI
    if ui.disconnectPressed()
        disp('Disconnect by user.')
        com.disconnect();
        return
    end
    
    % Update user interface
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