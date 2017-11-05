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
        
        % Updates UI with given robot data
        function update(obj, rd)
            obj.fig.Position = obj.FIG_POS;
            
            if nargin > 1
                uH = [sin(rd.heading); cos(rd.heading)];
                hold off
                plot([0 uH(1)], [0 uH(2)], 'color', 'b', 'linewidth', 2)
                hold on
                text(uH(1), uH(2), 'Heading')
                axis equal
                axis([-1.1, 1.1, -1.1, 1.1])
                grid on
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

