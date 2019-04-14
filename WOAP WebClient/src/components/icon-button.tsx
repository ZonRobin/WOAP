import * as React from "react";
import FontAwesomeIcon from "@fortawesome/react-fontawesome";
import {DragHandleProps} from "react-beautiful-dnd";
import {ClickableComponent} from "./clickable-component";

interface IconButtonProps {
    faIcon: any;
    className?: string;
    active?: boolean;
    onClick?: () => void;
}

export default class IconButton extends ClickableComponent<IconButtonProps, {}> {
    static defaultProps = {
        faIcon: {},
        className: "",
        active: false,
        onClick: () => {
        }
    };

    public clicked(): void {
        this.props.onClick();
    }

    public shouldComponentUpdate(newProps: IconButtonProps) {
        return this.props.faIcon !== newProps.faIcon
            || this.props.active !== newProps.active
            || this.props.onClick !== newProps.onClick
            || this.props.className !== newProps.className;
    }

    public render() {
        return (
            <button className={"iconButton " + this.props.className + (this.props.active ? " active" : "")}
                    type="button"
                    {...this.getClickableEventProps()}
            >
                <FontAwesomeIcon icon={this.props.faIcon}/>
            </button>
        );
    }
}