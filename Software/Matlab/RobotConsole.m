%---------------------------------------------------------------%
% RobotConsole.m
% Control and monitoring console for final project robot.
% RBE-2001 B17 Team 10
%---------------------------------------------------------------%

%% Clear Workspace
close all
clear vars
clc

%% User Interface Initialization
displayTitle();
ui = RobotUI();
com = RobotComms('Arduino', 1);

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

%% Disconnect from Robot
com.disconnect();

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