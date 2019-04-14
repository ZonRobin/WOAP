import * as React from "react";
import * as PropTypes from "prop-types";

interface PAGParameterNameState {

}

interface PAGParameterNameProps {

}

export default class PAGParameterName extends React.Component<PAGParameterNameProps, PAGParameterNameState> {
    static contextTypes = {
        parameterName: PropTypes.string
    };

    public shouldComponentUpdate(props, state, context) {
        return context.parameterName !== this.context.parameterName;
    }

    public render() {
        return (<span className="pagParameterName">{this.context.parameterName}</span>);
    }
}
