import * as React from "react";
import * as PropTypes from "prop-types";

interface PAGParameterValueState {

}

interface PAGParameterValueProps {

}

export default class PAGParameterValue extends React.Component<PAGParameterValueProps, PAGParameterValueState> {
    static contextTypes = {
        parameterValueText: PropTypes.string
    };

    public shouldComponentUpdate(props, state, context) {
        return context.parameterValueText !== this.context.parameterValueText;
    }

    public render() {
        return (<span className="pagParameterValue">{this.context.parameterValueText}</span>);
    }
}
