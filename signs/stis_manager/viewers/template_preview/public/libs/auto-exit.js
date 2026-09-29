import wincmd from 'node-windows';

setInterval(() => {
    wincmd.list((tasks) => {
        if (!Array.from(tasks).find((task) => task.ImageName == 'STISManager.exe')) {
            process.kill(process.pid);
        }
    });
}, 1000);
