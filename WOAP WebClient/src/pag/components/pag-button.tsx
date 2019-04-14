import * as React from "react";
import * as PropTypes from "prop-types";
import ControlComponentColorScheme from "../control-components/pag-control-component";
import Button from "../control-components/pag-button-control-component";
import {ArrayExtended} from "../../array-extended";

interface PAGButtonState {
}

interface PAGButtonProps {
    width?: number;
    height?: number;
    text?: string;
    colorScheme?: ControlComponentColorScheme;
    inverted?: boolean;
}

export default class PAGButton extends React.Component<PAGButtonProps, PAGButtonState> {
    static contextTypes = {
        parameterValueSetterCallback: PropTypes.func,
        parameterValue: PropTypes.number,
        parameterValueType: PropTypes.string,
        parameterSteps: PropTypes.instanceOf(ArrayExtended)
    };

    static defaultProps = {
        width: 50,
        height: 50,
        text: "S",
        colorScheme: ControlComponentColorScheme.getDefault(),
        inverted: false
    };

    public shouldComponentUpdate(props, state, context) {
        return context.parameterValue !== this.context.parameterValue;
    }

    public render() {
        return (
            <Button valueChangedCallback={this.context.parameterValueSetterCallback}
                    value={this.context.parameterValue}
                    width={this.props.width}
                    height={this.props.height}
                    text={this.props.text}
                    inverted={this.props.inverted}
                    steps={this.context.parameterSteps}
                    colorScheme={this.props.colorScheme}
            />);
    }
}
