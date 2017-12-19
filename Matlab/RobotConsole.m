%ROBOTCONSOLE Function for RBE-2002 Final Project Robot
%   Created by Dan Oates (RBE-2002 B17 Team 10).
%   
%   This function runs the robot user-interface, Bluetooth communication,
%   and field mapping. It can also 'replay' the previous field run from the
%   file 'RobotLog.mat'.
%
%   See also: ROBOTDATA, ROBOTUI, ROBOTCOMMS, MAPBUILDER

%% Initialization
% Clear workspace
close all
clear all
clc

% Create interface objects
ui = RobotUI();
robot = RobotComms('Arduino', 1);
map = MapBuilder();
logName = 'RobotLog.mat';

%% First User Input
% Options:
% - Connect to robot
% - Replay last robot run
% - Terminate program
replay = 0;
while 1
    displayTitle('Awaiting Input');

    % Connect Button Pressed
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
        
    % Replay Button Pressed
    elseif ui.replayButton()
        disp('Replaying last robot run...')
        replay = 1;
        pause(1)
        break
        
    % Disconnect Button Pressed
    elseif ui.disconnectButton()
        disp('Program terminated.')
        return
        
    % Text Prompt
    else
        disp('Press ''Connect'' to connect to robot.')
        disp('Press ''Replay'' to simulate last run.')
        disp('Press ''Disconnect'' to terminate program.')
    end

    ui.update();
end

%% Second User Input
% Only runs if robot is connected to via Bluetooth.
% Options:
% - Begin robot operations
% - Disconnect from robot
if ~replay
    while 1
        displayTitle('Awaiting Input');

        % Begin Button Pressed
        if ui.beginButton()
            [s, error] = robot.start();
            if s == 0
                disp(error)
                robot.disconnect();
                return
            end
            break
            
        % Disconnect Button Pressed
        elseif ui.disconnectButton()
            disp('Disconnected.')
            robot.disconnect();
            return
            
        % Text Prompt
        else 
            disp('Press ''Begin'' to begin autonomous.')
            disp('Press ''Disconnect'' to terminate program.')
        end

        ui.update();
    end
end

%% Robot Loop (Autonomous or Replay)
% Initialization
loop = 1;
if ~replay
    % Create empty array of robot data for log
    maxLoops = 10000;
    robotLog = RobotData.empty(0, maxLoops);
else
    % Load robot log from Matlab file
    logFile = load(logName, 'robotLog');
    robotLog = logFile.robotLog;
    clear('logFile')
    maxLoops = length(robotLog);
end

% Loop
while 1
    if ~replay
        % Autonomous Loop
        displayTitle('Autonomous Mode');
        disp(['Loop: ' int2str(loop) '/' int2str(maxLoops)])
        disp(' ')
        
        % Get robot data and update map
        [rd, s, error] = robot.getData();
        if s == 0
            disp(error)
            robot.disconnect();
            break
        end
        
        % Add robot data to log
        robotLog(loop) = rd;
    else
        % Replay Loop
        displayTitle('Replay Mode');
        disp(['Loop: ' int2str(loop) '/' int2str(maxLoops)])
        disp(' ')
        
        % Get recorded robot data from log
        rd = robotLog(loop);
    end

    % Update map if robot is wall-following
    if rd.isWallFollowing()
        map.update(rd);
    end
    
    % Display Robot Information
    disp('STATES')
    disp(['Robot: ' rd.robotState])
    disp(['Wall:  ' rd.wallFollowerState])
    disp(' ')
    
    disp('ROBOT')
    disp(['x: ' num2str(rd.pos(1), '%+.2f') ' [m]'])
    disp(['y: ' num2str(rd.pos(2), '%+.2f') ' [m]'])
    disp(['h: ' num2str(rad2deg(rd.heading), '%.0f') ' [deg]'])
    disp(['Alignment: ' rd.getAlignment()])
    disp(' ')
    
    disp('FLAME')
    disp(['Status: ' rd.flameStatus])
    disp('Position: ')
    disp(['x: ' num2str(rd.flamePos(1) * 39.3701, '%+.1f') ' [in]'])
    disp(['y: ' num2str(rd.flamePos(2) * 39.3701, '%+.1f') ' [in]'])
    disp(['z: ' num2str(rd.flamePos(3) * 39.3701, '%+.1f') ' [in]'])
    disp(' ')
    
    % Update UI and plots
    cla
    hold on
    rd.plot();
    map.plot();
    if ~isequal(rd.flamePos, [0; 0; 0])
        plotFlame(rd.flamePos);
    end
    ui.update();
    hold off
    
    % Check disconnect button
    if ui.disconnectButton()
        if replay
            disp('Simulation cancelled.')
            break
        else
            disp('Disconnect by user.')
            robot.disconnect();
            break
        end
    end
    
    % Increment loop count or exit if at max
    if loop == maxLoops
        disp('Max communication loops reached!')
        if ~replay
            robot.disconnect();
        end
        break
    else
        loop = loop + 1;
    end
end

%% Post Robot Loop

% Save robot log if not a replay
if ~replay
    save(logName, 'robotLog');
    disp(['Robot log saved in ''' logName ''''])
end

%% Support Functions

function displayTitle(subtitle)
    % Clears command line and displays program title and subtitle
    clc
    disp('-------------------------------------------------')
    disp('ROBOT CONSOLE')
    disp(subtitle)
    disp('-------------------------------------------------')
end

function plotFlame(pos)
    % Plots flame given position in [x; y; z] format
    x0 = pos(1) - 0.06;
    x1 = pos(1) + 0.06;
    y0 = pos(2) - 0.06;
    y1 = pos(2) + 0.06;
    xPlot = [x0 x1 x1 x0 x0];
    yPlot = [y0 y0 y1 y1 y0];
    plot(xPlot, yPlot, '-r')
end