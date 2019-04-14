import * as React from "react";
import FontAwesomeIcon from "@fortawesome/react-fontawesome";
import * as faSpinner from "@fortawesome/fontawesome-free-solid/faSpinner";

interface LoadingSpinnerProps {
    dark?: boolean;
    size?: number;
    overlay?: boolean;
    className?: string;
}

export default class LoadingSpinner extends React.Component<LoadingSpinnerProps, {}> {
    protected icon = <FontAwesomeIcon icon={faSpinner} spin/>;

    static defaultProps = {
        dark: false,
        size: 30,
        overlay: false,
        className: ""
    };

    public shouldComponentUpdate(props: LoadingSpinnerProps) {
        return this.props.dark !== props.dark
            || this.props.size !== props.size
            || this.props.overlay !== props.overlay
            || this.props.className !== props.className;
    }

    public render() {
        return (
            <div
                className={"loadingSpinner " + this.props.className + (this.props.dark ? " dark" : "") + (this.props.overlay ? " overlay" : "")}
                style={{fontSize: this.props.size}}>
                {this.icon}
            </div>);
    }
}
