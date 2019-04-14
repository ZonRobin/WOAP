import * as React from "react";
import * as PropTypes from "prop-types";
import LinearSlider from "../control-components/pag-linear-slider-control-component";
import ControlComponentColorScheme from "../control-components/pag-control-component";
import {ArrayExtended} from "../../array-extended";
import {PluginParameterValueType} from "../../models/plugin-parameter-model";

interface PAGLinearSliderState {
}

interface PAGLinearSliderProps {
    width?: number;
    height?: number;
    colorScheme?: ControlComponentColorScheme;
    inverted?: boolean;
    valueBar?: boolean;
}

export default class PAGLinearSlider extends React.Component<PAGLinearSliderProps, PAGLinearSliderState> {
    static contextTypes = {
        parameterValueSetterCallback: PropTypes.func,
        parameterValue: PropTypes.number,
        parameterValueType: PropTypes.string,
        parameterSteps: PropTypes.instanceOf(ArrayExtended),
        parameterBeginGestureCallback: PropTypes.func,
        parameterEndGestureCallback: PropTypes.func
    };

    static defaultProps = {
        width: 50,
        height: 50,
        colorScheme: ControlComponentColorScheme.getDefault(),
        inverted: false,
        valueBar: true
    };

    public shouldComponentUpdate(props, state, context) {
        return context.parameterValue !== this.context.parameterValue;
    }

    public render() {
        if (this.context.parameterValueType === PluginParameterValueType.Float) {
            return <LinearSlider valueChangedCallback={this.context.parameterValueSetterCallback}
                                 value={this.context.parameterValue}
                                 width={this.props.width}
                                 height={this.props.height}
                                 inverted={this.props.inverted}
                                 colorScheme={this.props.colorScheme}
                                 valueBar={this.props.valueBar}
                                 beginGestureCallback={this.context.parameterBeginGestureCallback}
                                 endGestureCallback={this.context.parameterEndGestureCallback}
            />;
        } else {
            return <LinearSlider valueChangedCallback={this.context.parameterValueSetterCallback}
                                 value={this.context.parameterValue}
                                 width={this.props.width}
                                 height={this.props.height}
                                 steps={this.context.parameterSteps}
                                 inverted={this.props.inverted}
                                 colorScheme={this.props.colorScheme}
                                 valueBar={this.props.valueBar}
                                 beginGestureCallback={this.context.parameterBeginGestureCallback}
                                 endGestureCallback={this.context.parameterEndGestureCallback}
            />;
        }
    }
}
