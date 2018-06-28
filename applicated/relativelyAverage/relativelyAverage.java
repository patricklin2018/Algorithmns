package relativelyAverage;

/**
 * @author PatrickLin
 * @date 2018/6/28 0:50
 * @description 计算相对平均的随机数数组 - 削骨填峰法
 *
 **/
public class relativelyAverage {

    /**
     * 整数版本
     * @param total    总额
     * @param n        份数
     * @param correct  误差
     * @return         各个份数组成的数组
     */
    public static int[] relativelyAverage(int total, int n, int correct) {
        // 计算波动边界
        int halfCorrect = correct / 2;
        int lowBound = total / n - halfCorrect;
        int highBound = lowBound + correct;

        // 将数组进行范围内随机初始化
        int[] result = new int[n];
        for (int i = 0; i < n; ++i) {
            result[i] = lowBound + (int)((correct + 1) * Math.random());
        }

        // 那么，剩余误差还有：
        int residue = 0;
        for (int i = 0; i < n; ++i) {
            residue += result[i];
        }
        residue = total - residue;
        while (residue != 0) {
            int wave;
            if (Math.abs(residue) < correct) {
                wave = residue;
            }
            else {
                wave = (int)(Math.random() * (correct + 1));
            }

            // 随机选取数组中的一个值
            int picked = (int)(Math.random() * n);
            if (result[picked] >= lowBound && result[picked] + wave <= highBound) {
                result[picked] = result[picked] + wave;
                residue = residue - wave;
            }
        }

        return result;
    }

    /**
     * 浮点数版本
     * @param total    总额
     * @param n        份数
     * @param correct  误差
     * @return         各个份数组成的数组
     */
    public static double[] relativelyAverage(double total, int n, double correct) {
        // 计算波动边界
        double halfCorrect = correct / 2;
        double lowBound = total / n - halfCorrect;
        double highBound = lowBound + correct;

        // 将数组进行范围内随机初始化
        double[] result = new double[n];
        for (int i = 0; i < n; ++i) {
            result[i] = lowBound + ((correct + 1) * Math.random());
        }

        // 那么，剩余误差还有：
        double residue = 0;
        for (int i = 0; i < n; ++i) {
            residue += result[i];
        }
        residue = total - residue;
        while (residue != 0) {
            double wave;
            if (Math.abs(residue) < correct) {
                wave = residue;
            }
            else {
                wave = Math.random() * (correct + 1);
            }

            // 随机选取数组中的一个值
            int picked = (int)(Math.random() * n);
            if (result[picked] >= lowBound && result[picked] + wave <= highBound) {
                result[picked] = result[picked] + wave;
                residue = residue - wave;
            }
        }

        return result;
    }

    public static void main(String[] args) {

        double[] ary = relativelyAverage(100.0, 5, 9.0);
        for (int i = 0; i < ary.length; ++i) {
            System.out.print(ary[i] + " ");
        }
    }

}
