classdef RobotUI < handle
    %ROBOTUI Summary of this class goes here
    %   Detailed explanation goes here
    
    properties (Access = private)
        fig;
        titleText;
        connectButton;
        beginButton;
        disconnectButton;
        plotAxes;
        
        cbPressed = 0;
        bbPressed = 0;
        dbPressed = 0;
        
        FIG_POS = [809, 49, 784, 768];
        
        ROBOT_RADIUS = 0.125; % (m)
        SONAR_OFFSET_F = [0; +0.1423];
        SONAR_OFFSET_B = [0; -0.1309];
        SONAR_OFFSET_L = [-0.1325; 0];
        SONAR_OFFSET_R = [+0.1288; 0];
    end
    
    methods (Access = public)
        
        % Class constructor
        function obj = RobotUI()
            obj.fig = figure(...
                'Name', 'Robot Console', ...
                'Position', obj.FIG_POS, ...
                'Resize', 'off');
            figure(obj.fig)
            
            obj.titleText = uicontrol(...
                'Style', 'text', ...
                'String', 'Robot Control Station', ...
                'BackgroundColor', [0.5, 0.5, 0.5], ...
                'FontSize', 24, ...
                'FontWeight', 'bold', ...
                'Units', 'normalized', ...
                'Position', obj.figPos([0.05 0.95 0.02 0.08]));
            
            obj.connectButton = uicontrol(...
                'Style', 'pushbutton', ...
                'String', 'Connect', ...
                'Units', 'normalized', ...
                'Position', obj.figPos([0.05, 0.15, 0.10, 0.13]), ...
                'Callback', @obj.cbCallback);
            
            obj.beginButton = uicontrol(...
                'Style', 'pushbutton', ...
                'String', 'Begin', ...
                'Units', 'normalized', ...
                'Position', obj.figPos([0.05, 0.15, 0.14, 0.17]), ...
                'Callback', @obj.bbCallback);
            
            obj.disconnectButton = uicontrol(...
                'Style', 'pushbutton', ...
                'String', 'Disconnect', ...
                'Units', 'normalized', ...
                'Position', obj.figPos([0.05, 0.15, 0.18, 0.21]), ...
                'Callback', @obj.dbCallback);
            
            obj.plotAxes = axes(...
                'XGrid', 'on', ...
                'YGrid', 'on', ...
                'OuterPosition', obj.figPos([0.05 0.95 0.25 0.95]));
            title('Field Plot');
            xlabel('X Position (m)')
            ylabel('Y Position (m)')
            
            drawnow
        end
        
        % Updates UI with given robot odometry data
        function update(obj, odm)
            obj.fig.Position = obj.FIG_POS;
            
            % If robot data is given
            if nargin > 1
                vH = obj.ROBOT_RADIUS * [sin(odm.h); cos(odm.h)];
                
                % Plot robot
                hold off
                plot(odm.x, odm.y, 'o', 'color', 'b')
                hold on
                plot(odm.x + [0 vH(1)], odm.y + [0 vH(2)], ...
                    'color', 'b', 'linewidth', 2)
                
                % Compute sonar vectors
                robotPos = [odm.x; odm.y];
                sh = sin(odm.h);
                ch = cos(odm.h);
                rotator = [ch sh; -sh ch];
                wF = robotPos + rotator * (obj.SONAR_OFFSET_F + [0; +odm.dF]);
                wB = robotPos + rotator * (obj.SONAR_OFFSET_B + [0; -odm.dB]);
                wL = robotPos + rotator * (obj.SONAR_OFFSET_L + [-odm.dL; 0]);
                wR = robotPos + rotator * (obj.SONAR_OFFSET_R + [+odm.dR; 0]);
                
                % Plot sonar vectors
                plot(wF(1), wF(2), 'x', 'color', 'r')
                plot(wB(1), wB(2), 'x', 'color', 'r')
                plot(wL(1), wL(2), 'x', 'color', 'r')
                plot(wR(1), wR(2), 'x', 'color', 'r')
                
                % Center axis on robot and show grid
                axis equal
                axis([-2.0 2.0 -2.0 2.0])
                grid on
                
                % Label the plot
                title('Field Plot');
                xlabel('X Position (m)')
                ylabel('Y Position (m)')
            end
            
            drawnow
        end
        
        % Returns if connect button has been pressed
        function p = connectPressed(obj)
            p = obj.cbPressed;
            obj.cbPressed = 0;
        end
        
        % Returns if begin button has been pressed
        function p = beginPressed(obj)
            p = obj.bbPressed;
            obj.bbPressed = 0;
        end
        
        % Returns if disconnect button has been pressed
        function p = disconnectPressed(obj)
            p = obj.dbPressed;
            obj.dbPressed = 0;
        end
        
    end
    
    methods (Access = private)
       
        % Takes my figure position style and converts it to Matlab's
        function pf = figPos(obj, pi)
            pf = zeros(1, 4);
            pf(1) = pi(1);
            pf(2) = 1 - pi(4);
            pf(3) = pi(2) - pi(1);
            pf(4) = pi(4) - pi(3);
        end
        
        % Callback for connect button
        function cbCallback(obj, source, event)
            obj.cbPressed = 1;
        end
        
        % Callback for begin button
        function bbCallback(obj, source, event)
            obj.bbPressed = 1;
        end
        
        % Callback for disconnect button
        function dbCallback(obj, source, event)
            obj.dbPressed = 1;
        end
    end
end

