import * as React from "react";
import FontAwesomeIcon from "@fortawesome/react-fontawesome";
import * as faBars from "@fortawesome/fontawesome-free-solid/faBars";
import {DragHandleProps} from "react-beautiful-dnd";
import {ClickableComponent} from "./clickable-component";

interface Props {
    dragHandleProps: DragHandleProps;
    onClick?: () => void;
}

export default class DragHandle extends ClickableComponent<DragHandleProps, {}> {
    protected icon = <FontAwesomeIcon icon={faBars}/>;

    static defaultProps = {
        dragHandleProps: {},
        onClick: () => {
        }
    };

    public clicked(): void {
        this.props.onClick();
    }

    public render() {
        let clickableProps = this.getClickableEventProps();
        let finalProps = {...this.props.dragHandleProps};
        finalProps.onMouseDown = (ev) => {
            clickableProps.onMouseDown(ev);
            this.props.dragHandleProps.onMouseDown(ev);
        };
        finalProps.onTouchStart = (ev) => {
            clickableProps.onTouchStart(ev);
            this.props.dragHandleProps.onTouchStart(ev);
        };
        finalProps.onMouseUp = clickableProps.onMouseUp;
        finalProps.onTouchEnd = clickableProps.onTouchEnd;

        return (
            <div className="dragHandle" data-scroll-ignore {...finalProps}>
                {this.icon}
            </div>
        );
    }
}