export default class ErrorBoundary extends React.Component {
    constructor(props) {
        super(props);
        this.state = { failed: false };
    }

    static getDerivedStateFromError(error) {
        return { failed: true };
    }

    render() {
        if (this.state.failed) {
            return React.createElement("div", { className: "alert alert-danger" },
                React.createElement("h4", { className: "alert-heading" }, "Error"),
                React.createElement("p", null, this.props.message));
        }

        return this.props.children;
    }
}
