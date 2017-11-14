%---------------------------------------------------------------%
% RobotConsole.m
% Control and monitoring console for final project robot.
% RBE-2001 B17 Team 10
%---------------------------------------------------------------%

%% Clear Workspace
close all
clear
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
    [rd, s, error] = com.getData();
    if s ~= 1
        disp(error)
        com.disconnect();
        return
    end
    
    % Check disconnect button on UI
    if ui.disconnectPressed()
        disp('Disconnect by user.')
        com.disconnect();
        return
    end
    
    % Update user interface
    ui.update(rd);
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