%ROBOTCONSOLE Function for RBE-2002 Final Project Robot
%   Created by RBE-2002 B17 Team 10.
%   
%   This function handles the robot user-interface, Bluetooth,
%   and SLAM for field mapping and position correction.
%
%   See also: ROBOTUI, ROBOTCOMMS, ROBOTDATA, MAPBUILDER

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
    elseif ui.disconnectButton()
        disp('Program terminated.')
        return
    else
        disp('Press ''Connect'' to connect to robot.')
        disp('Press ''Replay'' to simulate last run.')
        disp('Press ''Disconnect'' to terminate program.')
    end

    ui.update();
end

%% Second User Input
% Runs only if robot is connected to.
% Options:
% - Begin robot operations
% - Disconnect from robot
if ~replay
    while 1
        displayTitle('Awaiting Input');

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
    maxLoops = 10000;
    robotLog = RobotData.empty(0, maxLoops);
else
    logFile = load(logName, 'robotLog');
    robotLog = logFile.robotLog;
    clear('logFile')
    maxLoops = length(robotLog);
end

% Loop
while 1
    if ~replay
        % Autonomous loop
        displayTitle('Autonomous Mode');
        disp(['Loop: ' int2str(loop) '/' int2str(maxLoops)])
        disp(' ')
        
        % Get odometry data and update map
        [rd, s, error] = robot.getData();
        if s == 0
            disp(error)
            robot.disconnect();
            break
        end
        robotLog(loop) = rd;
    else
        % ReplaylLoop
        displayTitle('Replay Mode');
        disp(['Loop: ' int2str(loop) '/' int2str(maxLoops)])
        disp(' ')
        
        % Get recorded robot data
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

% Save robot log for replays
if ~replay
    save(logName, 'robotLog');
    disp(['Robot log saved in ''' logName ''''])
end

%% Helper Functions

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