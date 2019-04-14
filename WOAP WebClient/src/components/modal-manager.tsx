import * as React from "react";
import Modal from "./modal";
import {ArrayExtended} from "../array-extended";

interface ModalManagerState {
    modals: ArrayExtended<React.ReactElement<any>>;
}

interface ModalManagerProps {

}

export default class ModalManager extends React.Component<ModalManagerProps, ModalManagerState> {
    protected nextModalKey: number = 0;

    public constructor(props) {
        super(props);

        this.state = {
            modals: new ArrayExtended()
        };
    }

    public componentWillMount() {
        window.WOAP.ServiceLocator.setModalManager(this);
    }

    public componentWillUnmount() {
        window.WOAP.ServiceLocator.setModalManager(null);
    }

    public openModalIfNotExists(content: React.ReactElement<any>, id: string): boolean {
        if (!this.doesModalExist(id)) {
            this.openModal(content, id);
            return true;
        }

        return false;
    }

    public openModal(content: React.ReactElement<any>, id: string): void {
        this.state.modals.add(<Modal
            content={content}
            id={id}
            key={this.nextModalKey++}
        />);

        this.forceUpdate();
    }

    public closeModal(modalId: string): void {
        this.state.modals.findAndRemove((item) => item.props.id === modalId);
        this.forceUpdate();
    }

    public doesModalExist(modalId: string): boolean {
        return null !== this.state.modals.find((item) => item.props.id === modalId);
    }

    public setModalOnTop(modalId: string): void {
        if (this.state.modals.getLength() === 0)
            return;

        if (this.state.modals.getLast().props.id === modalId)
            return;

        let index = this.state.modals.findIndex((item) => item.props.id === modalId);
        if (index < 0)
            return;

        this.state.modals.moveAtIndex(index, -1);
        this.forceUpdate();
    }

    public shouldComponentUpdate() {
        return false;
    }

    public render() {
        return (<div className="modalManager">{this.state.modals.toArray()}</div>);
    }
}
