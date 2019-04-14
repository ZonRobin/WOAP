import * as React from "react";
import {Utility} from "../../utility";
import {
    PAGControlComponentColorSchemes,
    PAGControlComponentProps,
    PAGControlComponentState
} from "./pag-control-component";
import {ArrayExtended} from "../../array-extended";

interface PAGButtonControlComponentProps extends PAGControlComponentProps {
    text: string;
}

export default class PAGButtonControlComponent extends React.Component<PAGButtonControlComponentProps, PAGControlComponentState> {
    protected controlId: string;

    static defaultProps = {
        value: 0,
        width: 50,
        height: 50,
        inverted: false,
        text: "S",
        steps: new ArrayExtended<number>(),
        colorScheme: PAGControlComponentColorSchemes.grey,
        valueChangedCallback: (value: number) => {
        },
        beginGestureCallback: () => {
        },
        endGestureCallback: () => {
        }
    };

    protected toggleValueBound;

    public constructor(props) {
        super(props);
        this.controlId = "control-" + Utility.randDigits(10);
        this.toggleValueBound = this.toggleValue.bind(this);
    }

    public shouldComponentUpdate(newProps: PAGControlComponentProps, newState: PAGControlComponentState) {
        return this.props.value !== newProps.value;
    }

    public invertValueIfNeeded(value: number): number {
        if (this.props.inverted) {
            return 1 - value;
        }

        return value;
    }

    public toggleValue() {
        let value = this.invertValueIfNeeded(this.props.value);
        let newValue;

        if (this.props.steps.getLength() === 2) {
            let closestIndex = this.props.steps.findClosest((item) => item - value);
            newValue = this.props.steps.get(1 - closestIndex, 0);
        } else {
            if (this.props.value > 0.5) {
                newValue = 0;
            } else {
                newValue = 1;
            }
        }
        this.props.valueChangedCallback(newValue);
    }

    public isToggled() {
        let value = this.invertValueIfNeeded(this.props.value);

        if (this.props.steps.getLength() === 2) {
            let closestIndex = this.props.steps.findClosest((item) => item - value);
            let otherValue = this.props.steps.get(1 - closestIndex, 0);
            return otherValue < value;
        } else {
            return value > 0.5;
        }
    }

    public render() {
        let className = "parameterControl parameterControlButton";
        let style = {
            backgroundColor: this.props.colorScheme.fill,
            color: this.props.colorScheme.accent,
            border: "1px solid " + this.props.colorScheme.accent,
            width: this.props.width,
            height: this.props.height,
            lineHeight: this.props.height + "px",
            fontSize: this.props.height * 0.6,
            paddingTop: -2,
            borderRadius: 3
        };

        if (this.isToggled()) {
            className += " toggled";
            style.backgroundColor = this.props.colorScheme.accent;
            style.color = this.props.colorScheme.fill;
            style.border = "1px solid " + this.props.colorScheme.fill;
        }

        return (
            <div className={className}
                 id={this.controlId}
                 onClick={this.toggleValueBound}
                 style={style}
            >
                {this.props.text}
            </div>);
    }
}