import * as React from "react";
import {ClickableComponent} from "./clickable-component";

interface ButtonListProps {
    buttons: Array<[any, string]>;
    onItemClick: (any) => void;
}

interface ButtonListState {

}

export default class ButtonList extends React.Component<ButtonListProps, ButtonListState> {

    static defaultProps = {
        buttons: [],
        onClick: () => {
        }
    };

    public shouldComponentUpdate(props: ButtonListProps) {
        if (this.props.buttons.length !== props.buttons.length)
            return false;

        for (let i = 0; i < props.buttons.length; ++i) {
            if (this.props.buttons[i][0] !== props.buttons[i][0]
                || this.props.buttons[i][1] !== props.buttons[i][1])
                return false;
        }

        return true;
    }

    public render() {
        return (
            <div className="buttonList">
                {this.props.buttons.map((item: [any, string]) =>
                    <ButtonListItem key={"item" + item[0]} id={item[0]} text={item[1]} onClick={this.props.onItemClick}/>
                )}
            </div>);
    }
}

interface ButtonListItemProps {
    id: any;
    text: string;
    onClick: (any) => void;
}

export class ButtonListItem extends ClickableComponent<ButtonListItemProps, {}> {

    public clicked(): void {
        this.props.onClick(this.props.id);
    }

    public shouldComponentUpdate(props: ButtonListItemProps) {
        return this.props.id !== props.id
            || this.props.text !== props.text;
    }

    public render() {
        return (
            <button {...this.getClickableEventProps()}
                    className="buttonListItem"
                    type="button"
            >
                {this.props.text}
            </button>);
    }
}