import * as React from "react";
import {
    ApiCoreAuthenticationResponseMessage,
    ApiCoreAuthenticateMessage,
    ApiCoreLoginMessage,
    ApiCoreLoginResponseMessage
} from "./api/messages/app-messages";
import App from "./app";

export interface LoginProps {

}

export interface LoginState {
    password: string;
    authenticating: boolean;
}

export default class Login extends React.Component<LoginProps, LoginState> {
    protected onPasswordChange;
    protected onSubmit;

    constructor(props) {
        super(props);
        this.state = {
            password: "",
            authenticating: true
        };

        this.onPasswordChange = this.passwordChanged.bind(this);
        this.onSubmit = this.submitRequested.bind(this);
    }

    public componentDidMount() {
        this.tryAuthenticate();
    }

    public tryAuthenticate() {
        let logTag = window.WOAP.ServiceLocator.getStoresManager().SystemStore.getLogTag();
        if (logTag.length === 0) {
            this.setState({authenticating: false});
            return;
        }

        window.WOAP.ServiceLocator.getAPIWorker()
            .request(new ApiCoreAuthenticateMessage().addArguments(logTag), ApiCoreAuthenticationResponseMessage.getAddress())
            .then((message) => {
                let msg = new ApiCoreAuthenticationResponseMessage(message);
                if (!msg.isValid()) {
                    return;
                }

                if (msg.getResponseCode() === ApiCoreAuthenticationResponseMessage.ResponseCode.Ok) {
                    window.WOAP.ServiceLocator.getStoresManager().SystemStore.setAuthenticated(true);
                } else {
                    window.WOAP.ServiceLocator.getStoresManager().SystemStore.setLogTag("");
                    this.setState({authenticating: false});
                }
            }).catch(() => App.showGeneralErrorBox());
    }

    public passwordChanged(event) {
        event.preventDefault();
        this.setState({password: event.target.value});
    }

    public submitRequested(event) {
        event.preventDefault();

        window.WOAP.ServiceLocator.getAPIWorker()
            .request(new ApiCoreLoginMessage().addArguments(this.state.password), ApiCoreLoginResponseMessage.getAddress())
            .then((message) => {
                let msg = new ApiCoreLoginResponseMessage(message);
                if (!msg.isValid()) {
                    return;
                }

                if (msg.getResponseCode() === ApiCoreLoginResponseMessage.ResponseCode.Ok) {
                    window.WOAP.ServiceLocator.getStoresManager().SystemStore
                        .setLogTag(msg.getLogTag())
                        .setAuthenticated(true);
                } else {
                    App.showErrorBox("Could not login: " + msg.getResponseString() + ".");
                }
            }).catch(() => App.showGeneralErrorBox());
    }

    public render() {
        return (
            <div className={"loginForm" + (this.state.authenticating ? " hidden" : "")}>
                <h1>WOAP web control</h1>
                <form onSubmit={this.onSubmit}>
                    <input type="password" name="password" placeholder="Password" value={this.state.password}
                           onChange={this.onPasswordChange}/>
                    <input type="submit" value="Login"/>
                </form>
            </div>);
    }
}