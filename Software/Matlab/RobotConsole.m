%ROBOTCONSOLE Script for RBE-2002 Final Project Robot
%   Created by RBE-2002 B17 Team 10.
%   
%   This script handles runs robot user-interface, Bluetooth communication,
%   and displays script and robot status via the Matlab command line.

%% Clear Workspace
close all
clear all
clc

%% Constants
BLUETOOTH_NAME = 'Arduino';
BLUETOOTH_CHANNEL = 1;
TELEOP_VOLTAGE = 4;
MAX_LOOPS = 5000;
LOG_NAME = 'RobotLog.mat';

%% User Interface Initialization
try
    xbox = XboxController(1, 0.1, 1.5);
catch
    displayTitle('Xbox controller not connected!')
    return
end
ui = RobotUI();
robot = RobotComms(BLUETOOTH_NAME, BLUETOOTH_CHANNEL);
map = MapBuilder();

%% Connect to Robot or Replay Log
replay = 0;
while 1
    displayTitle('Awaiting input');

    if ui.connectButton()
        disp('Connecting to robot...')
        [s, msg] = robot.connect();
        disp(msg)
        pause(1)
        if s == 1
            break
        else
            robot.disconnect();
            return
        end
    elseif ui.replayButton()
        disp('Replaying last robot run...')
        replay = 1;
        pause(1)
        break
    else
        disp('Press ''Connect'' to connect to robot.')
        disp('Press ''Replay'' to simulate last run.')
    end

    ui.update();
end

%% Begin Teleop or Disconnect
if ~replay
    while 1
        displayTitle('Awaiting input');

        if ui.beginButton()
            [s, error] = robot.start();
            if s == 0
                disp(error)
                robot.disconnect();
                return
            end
            break
        elseif ui.disconnectButton()
            disp('Disconnected.')
            robot.disconnect();
            return
        else 
            disp('Press ''Begin'' to begin teleop.')
            disp('Press ''Disconnect'' to terminate program.')
        end

        ui.update();
    end
end

%% Robot Loop (Teleop or Replay)
loopCount = 1;
if replay  
    load(LOG_NAME);
    MAX_LOOPS = length(robotLog);
else
    robotLog = RobotData.empty(0, MAX_LOOPS);
end
targetHeading = 0;

while 1
    displayTitle('Teleoperated Mode');
    disp(['Loop: ' int2str(loopCount)])

    if ~replay
        % Control drive with Xbox
        dpad = xbox.dPad();
        if dpad ~= -1 && mod(dpad, 90) == 0
            targetHeading = dpad;
        end
        js = xbox.LJS();
        driveVoltage = js(2) * TELEOP_VOLTAGE;
        [s, error] = robot.drive(driveVoltage, targetHeading);
        if s == 0
            disp(error)
            robot.disconnect();
            break
        end
        
        % Get odometry data and update map
        [rd, s, error] = robot.getData();
        if s == 0
            disp(error)
            robot.disconnect();
            return
        end
        robotLog(loopCount) = rd;
        map.update(rd);

        % Check disconnect button on UI
        if ui.disconnectButton()
            disp('Disconnect by user.')
            robot.disconnect();
            break
        end
    else
        rd = robotLog(loopCount);
        map.update(rd);
    end

    % Update UI and plots
    cla
    hold on
    rd.plot();
    map.plot();
    ui.update();
    hold off
    
    % Increment loop count or exit if at max
    if loopCount == MAX_LOOPS
        disp('Max communication loops reached!')
        if ~replay
            robot.disconnect();
        end
        break
    else
        loopCount = loopCount + 1;
    end
end

%% Save robot log in replay file
if ~replay
    save(LOG_NAME, 'robotLog');
    disp(['Robot log saved in ''' LOG_NAME '.mat'''])
end

%% Helper Functions

function displayTitle(subtitle)
    % Clears command line and displays program title and subtitle
    clc
    disp('Robot Console')
    disp([subtitle newline])
end