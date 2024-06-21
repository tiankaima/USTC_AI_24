import numpy as np
import pandas as pd
from typing import Union, Optional
from sklearn.tree import DecisionTreeClassifier as SKDecisionTreeClassifier
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import LabelEncoder


# metrics
def accuracy(y_true: np.ndarray, y_pred: np.ndarray) -> float:
    return np.sum(y_true == y_pred) / len(y_true)


# model
class DecisionTreeNode:
    """
    Decision Tree Node, multiple children

    Attributes:
        feature: int, the feature index of the split
        threshold: float, the threshold value if feature is continuous
        values: dict, key: feature value, value: child node
        label: int, only leaf node has label, otherwise None
    """

    def __init__(self, feature: int, threshold: Optional[float] = None, values: Optional[dict] = None,
                 label: int = None) -> None:
        self.feature = feature
        self.threshold = threshold
        self.values = values if values is not None else {}
        self.label = label

    def is_leaf(self) -> bool:
        return self.label is not None

    @staticmethod
    def leaf(label: int) -> 'DecisionTreeNode':
        return DecisionTreeNode(feature=-1, label=label)

    def __repr__(self):
        return f"DecisionTreeNode(feature={self.feature}, threshold={self.threshold}, label={self.label})"


class DecisionTreeClassifier:
    def __init__(self) -> None:
        self.tree = None

    def entropy(self, y: np.ndarray) -> float:
        _, counts = np.unique(y, return_counts=True)
        probabilities = counts / counts.sum()
        return -np.sum(probabilities * np.log2(probabilities))

    def information_gain(self, y: np.ndarray, y_left: np.ndarray, y_right: np.ndarray) -> float:
        weight_left = len(y_left) / len(y)
        weight_right = len(y_right) / len(y)
        return self.entropy(y) - (weight_left * self.entropy(y_left) + weight_right * self.entropy(y_right))

    def best_split(self, X: pd.DataFrame, y: np.ndarray) -> tuple:
        best_feature = None
        best_threshold = None
        best_info_gain = -1

        for feature in X.columns:
            thresholds = np.unique(X[feature])
            for threshold in thresholds:
                y_left = y[X[feature] <= threshold]
                y_right = y[X[feature] > threshold]
                if len(y_left) == 0 or len(y_right) == 0:
                    continue

                info_gain = self.information_gain(y, y_left, y_right)
                if info_gain > best_info_gain:
                    best_info_gain = info_gain
                    best_feature = feature
                    best_threshold = threshold

        return best_feature, best_threshold

    def tree_generate(self, X: pd.DataFrame, y: np.ndarray) -> DecisionTreeNode:
        if len(set(y)) == 1:
            return DecisionTreeNode.leaf(int(y[0]))

        if X.shape[1] == 0:
            return DecisionTreeNode.leaf(int(np.bincount(y).argmax()))

        best_feature, best_threshold = self.best_split(X, y)
        if best_feature is None:
            return DecisionTreeNode.leaf(int(np.bincount(y).argmax()))

        mask_left = X[best_feature] <= best_threshold
        mask_right = X[best_feature] > best_threshold

        X_left, y_left = X[mask_left], y[mask_left]
        X_right, y_right = X[mask_right], y[mask_right]

        children = {
            'left': self.tree_generate(X_left, y_left),
            'right': self.tree_generate(X_right, y_right)
        }

        return DecisionTreeNode(best_feature, best_threshold, children)

    def fit(self, X: pd.DataFrame, y: np.ndarray):
        self.tree = self.tree_generate(X, y)

    def tree_predict(self, X: pd.DataFrame, node: DecisionTreeNode) -> np.ndarray:
        if node.is_leaf():
            return np.array([node.label] * X.shape[0])

        mask_left = X[node.feature] <= node.threshold
        mask_right = X[node.feature] > node.threshold

        result = np.zeros(X.shape[0])
        result[mask_left] = self.tree_predict(X[mask_left], node.values['left'])
        result[mask_right] = self.tree_predict(X[mask_right], node.values['right'])

        return result

    def predict(self, X):
        return self.tree_predict(X, self.tree)


def load_data(datapath: str = './data/ObesityDataSet_raw_and_data_sinthetic.csv'):
    df = pd.read_csv(datapath)
    continue_features = ['Age', 'Height', 'Weight']
    discrete_features = ['Gender', 'CALC', 'FAVC', 'FCVC', 'NCP', 'SCC', 'SMOKE', 'CH2O',
                         'family_history_with_overweight', 'FAF', 'TUE', 'CAEC', 'MTRANS']

    X, y = df.iloc[:, :-1], df.iloc[:, -1]
    # encode discrete str to number, eg. male&female to 0&1
    label_encoder = LabelEncoder()
    for col in discrete_features:
        X[col] = label_encoder.fit_transform(X[col])
    y = label_encoder.fit_transform(y)

    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

    return X_train, X_test, y_train, y_test


def main():
    X_train, X_test, y_train, y_test = load_data('./data/ObesityDataSet_raw_and_data_sinthetic.csv')
    clf = DecisionTreeClassifier()
    clf.fit(X_train, y_train)

    y_pred = clf.predict(X_test)
    print(accuracy(y_test, y_pred))

    sklearn_clf = SKDecisionTreeClassifier(criterion="entropy")
    sklearn_clf.fit(X_train, y_train)
    sklearn_y_pred = sklearn_clf.predict(X_test)
    print(accuracy(y_test, sklearn_y_pred))


if __name__ == '__main__':
    main()
