import fs from 'fs';
import path from 'path';
import express from 'express';
import serve_index from 'serve-index';
import express_handlebars from 'express-handlebars';
import morgan from 'morgan';
import * as glob from 'glob';
import worker_threads from 'worker_threads';
import minimist from 'minimist';

const args = minimist(process.argv.slice(2));

if (args['auto-exit']) {
    const worker = new worker_threads.Worker('./public/libs/auto-exit.js');
}

const PORT = args.port || process.env.STIS_TEMPLATE_PREVIEW_PORT || 10099;
const SNAPSHOT_DIR = path.resolve(args['snapshot-dir'] || process.env.STIS_SNAPSHOT_DIR || String.raw`C:\transActive\config\database\stis\tmlibrary\snapshot`);

const handlebars = express_handlebars.create({
    defaultLayout: 'main',
    helpers: {
        counter: function (index) {
            return index + 1;
        },
    },
});

const app = express();
app.set('port', PORT);
app.engine('handlebars', handlebars.engine);
app.set('view engine', 'handlebars');
app.set('view cache', true);
app.disable('x-powered-by');
app.use(morgan('tiny'));
app.use(express.static('public'));
app.use(express.static(SNAPSHOT_DIR));
app.use('/jquery', express.static('node_modules/jquery/dist'));
app.use('/handlebars', express.static('node_modules/handlebars/dist'));
app.use('/bootstrap', express.static('node_modules/bootstrap/dist'));
app.use('/snapshot', express.static(SNAPSHOT_DIR), serve_index(SNAPSHOT_DIR, { icons: true }));

app.get('/', (req, res) => {
    res.redirect('snapshot');
});

app.get(/^.(EMG1|EMG2|TMP3|TMP4)\d{3}.?$/i, async (req, res, next) => {
    let template_id = req.url.replace(/\//g, '').toUpperCase();
    let dir = path.resolve(path.join(SNAPSHOT_DIR, template_id));

    if (!fs.existsSync(dir)) {
        console.error('can not find', dir);
        return next();
    }

    let files = await glob.glob(`${dir}/*.bmp`);
    let file_names_without_default = files
        .map((f) => {
            return path.basename(f, '.bmp').substring(8);
        })
        .filter((name) => {
            return name != 'DEFAULT';
        });

    let file_names = ['DEFAULT', ...file_names_without_default];

    console.info(template_id, dir, file_names);

    res.render('template-carousel', {
        layout: null,
        template_id,
        file_names,
        file_names_without_default,
        is_lcd: /EMG1|TMP3/i.test(req.url),
        is_led: /EMG2|TMP4/i.test(req.url),
        default_picture: `/${template_id}/${template_id}-${file_names[0]}.bmp`,
    });
});

app.get('/about', (req, res) => {
    res.render('about');
});

app.get('/headers', (req, res) => {
    res.set('Content-Type', 'text/plain');
    let s = Object.keys(req.headers)
        .map((name) => `${name}: ${req.headers[name]}`)
        .join('\n');
    res.send(s);
});

app.use((req, res) => {
    res.status(404);
    res.render('404');
});

app.use((err, req, res, next) => {
    console.error(err.stack);
    res.status(500);
    res.render('500');
});

app.listen(PORT, () => {
    console.log(`TemplatePreview started on http://localhost:${PORT}`);
    console.log(`snapshot: ${SNAPSHOT_DIR}`);
});
