import matplotlib.pyplot as plt
import numpy as np
from gensim.models import KeyedVectors


def get_kernel_function(kernel: str):
    if kernel == "linear":
        return lambda X, Y: np.dot(X, Y.T)
    elif kernel == "poly":
        return lambda X, Y, p=3: (np.dot(X, Y.T) + 1) ** p
    elif kernel == "rbf":
        return lambda X, Y, sigma=5.0: np.exp(
            -np.linalg.norm(X[:, np.newaxis] - Y[np.newaxis, :], axis=2) ** 2 / (2 * sigma ** 2))
    else:
        raise ValueError(f"Unknown kernel: {kernel}")


class PCA:
    def __init__(self, n_components: int = 2, kernel: str = "rbf") -> None:
        self.n_components = n_components
        self.kernel = kernel
        self.kernel_f = get_kernel_function(kernel)
        self.X_fit = None
        self.alphas = None
        self.lambdas = None

    def fit(self, X: np.ndarray):
        self.X_fit = X
        K = self.kernel_f(X, X)
        N = K.shape[0]

        # Center the kernel matrix
        one_n = np.ones((N, N)) / N
        K_centered = K - one_n @ K - K @ one_n + one_n @ K @ one_n

        # Eigen decomposition
        eigvals, eigvecs = np.linalg.eigh(K_centered)
        sorted_indices = np.argsort(eigvals)[::-1]
        self.lambdas = eigvals[sorted_indices][:self.n_components]
        self.alphas = eigvecs[:, sorted_indices][:, :self.n_components]

    def transform(self, X: np.ndarray):
        K = self.kernel_f(X, self.X_fit)
        return K @ self.alphas / np.sqrt(self.lambdas)


class KMeans:
    def __init__(self, n_clusters: int = 3, max_iter: int = 10) -> None:
        self.n_clusters = n_clusters
        self.max_iter = max_iter
        self.centers = None
        self.labels = None

    def initialize_centers(self, points):
        n, d = points.shape
        self.centers = np.zeros((self.n_clusters, d))
        for k in range(self.n_clusters):
            random_index = np.random.choice(n, size=10, replace=False)
            self.centers[k] = points[random_index].mean(axis=0)
        return self.centers

    def assign_points(self, points):
        n_samples = points.shape[0]
        self.labels = np.zeros(n_samples)
        for i in range(n_samples):
            distances = np.linalg.norm(points[i] - self.centers, axis=1)
            self.labels[i] = np.argmin(distances)
        return self.labels

    def update_centers(self, points):
        for k in range(self.n_clusters):
            cluster_points = points[self.labels == k]
            if len(cluster_points) > 0:
                self.centers[k] = cluster_points.mean(axis=0)

    def fit(self, points):
        self.initialize_centers(points)
        for _ in range(self.max_iter):
            old_centers = self.centers.copy()
            self.assign_points(points)
            self.update_centers(points)
            if np.all(old_centers == self.centers):
                break

    def predict(self, points):
        return self.assign_points(points)


def load_data():
    words = [
        'computer', 'laptop', 'minicomputers', 'PC', 'software', 'Macbook',
        'king', 'queen', 'monarch', 'prince', 'ruler', 'princes', 'kingdom', 'royal',
        'man', 'woman', 'boy', 'teenager', 'girl', 'robber', 'guy', 'person', 'gentleman',
        'banana', 'pineapple', 'mango', 'papaya', 'coconut', 'potato', 'melon',
        'shanghai', 'HongKong', 'chinese', 'Xiamen', 'beijing', 'Guilin',
        'disease', 'infection', 'cancer', 'illness',
        'twitter', 'facebook', 'chat', 'hashtag', 'link', 'internet',
    ]
    w2v = KeyedVectors.load_word2vec_format('./data/GoogleNews-vectors-negative300.bin', binary=True)
    vectors = []
    for w in words:
        vectors.append(w2v[w].reshape(1, 300))
    vectors = np.concatenate(vectors, axis=0)
    return words, vectors


def main():
    words, data = load_data()
    pca = PCA(n_components=2)
    pca.fit(data)
    data_pca = pca.transform(data)

    kmeans = KMeans(n_clusters=7)
    kmeans.fit(data_pca)
    clusters = kmeans.predict(data_pca)

    plt.figure()
    plt.scatter(data_pca[:, 0], data_pca[:, 1], c=clusters, cmap='viridis')
    for i in range(len(words)):
        plt.annotate(words[i], (data_pca[i, 0], data_pca[i, 1]))
    plt.title("PB21000030")
    plt.savefig("PCA_KMeans.png")
    plt.show()


if __name__ == '__main__':
    main()
