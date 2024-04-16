#include "window.h"
#include <QCalendarWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLocale>
#include <QTextCharFormat>

Window::Window(QWidget *parent) : QWidget(parent) {
    createPreviewGroupBox();
    createGeneralOptionsGroupBox();
    createDatesGroupBox();
    createTextFormatsGroupBox();
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(previewGroupBox, 0, 0);
    layout->addWidget(generalOptionsGroupBox, 0, 1);
    layout->addWidget(datesGroupBox, 1, 0);
    layout->addWidget(textFormatsGroupBox, 1, 1);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(layout);
    previewLayout->setRowMinimumHeight(0, calendar->sizeHint().height());
    previewLayout->setColumnMinimumWidth(0, calendar->sizeHint().width());
    setWindowTitle(tr("Календарь"));
}

void Window::localeChanged(int index) {
    const QLocale newLocale(localeCombo->itemData(index).toLocale());
    calendar->setLocale(newLocale);
    int newLocaleFirstDayIndex = firstDayCombo->findData(newLocale.firstDayOfWeek());
    firstDayCombo->setCurrentIndex(newLocaleFirstDayIndex);
}

void Window::firstDayChanged(int index) {
    calendar->setFirstDayOfWeek(Qt::DayOfWeek(firstDayCombo->itemData(index).toInt()));
}

void Window::selectionModeChanged(int index) {
    calendar->setSelectionMode(QCalendarWidget::SelectionMode(selectionModeCombo->itemData(index).toInt()));
}

void Window::horizontalHeaderChanged(int index) {
    calendar->setHorizontalHeaderFormat(QCalendarWidget::HorizontalHeaderFormat(horizontalHeaderCombo->itemData(index).toInt()));
}

void Window::verticalHeaderChanged(int index) {
    calendar->setVerticalHeaderFormat(QCalendarWidget::VerticalHeaderFormat(verticalHeaderCombo->itemData(index).toInt()));
}

void Window::selectedDateChanged() {
    currentDateEdit->setDate(calendar->selectedDate());
}

void Window::minimumDateChanged(QDate date) {
    calendar->setMinimumDate(date);
    maximumDateEdit->setDate(calendar->maximumDate());
}

void Window::maximumDateChanged(QDate date) {
    calendar->setMaximumDate(date);
    minimumDateEdit->setDate(calendar->minimumDate());
}

void Window::weekdayFormatChanged() {
    QTextCharFormat format;
    format.setForeground(qvariant_cast<QColor>(weekdayColorCombo->itemData(weekdayColorCombo->currentIndex())));
    calendar->setWeekdayTextFormat(Qt::Monday, format);
    calendar->setWeekdayTextFormat(Qt::Tuesday, format);
    calendar->setWeekdayTextFormat(Qt::Wednesday, format);
    calendar->setWeekdayTextFormat(Qt::Thursday, format);
    calendar->setWeekdayTextFormat(Qt::Friday, format);
}

void Window::weekendFormatChanged() {
    QTextCharFormat format;
    format.setForeground(qvariant_cast<QColor>(weekendColorCombo->itemData(weekendColorCombo->currentIndex())));
    calendar->setWeekdayTextFormat(Qt::Saturday, format);
    calendar->setWeekdayTextFormat(Qt::Sunday, format);
}

void Window::reformatHeaders() {
    QString text = headerTextFormatCombo->currentText();
    QTextCharFormat format;
    if(text == tr("Жирный")) {
        format.setFontWeight(QFont::Bold);
    }
    else if(text == tr("Курсив")) {
        format.setFontItalic(true);
    }
    else if(text == tr("Зелёный")) {
        format.setForeground(Qt::green);
    }
    calendar->setHeaderTextFormat(format);
}

void Window::reformatCalendarPage() {
    QTextCharFormat mayFirstFormat;
    const QDate mayFirst(calendar->yearShown(), 5, 1);
    if(mayFirstCheckBox->isChecked()) {
        mayFirstFormat.setForeground(Qt::red);
    }
    calendar->setDateTextFormat(mayFirst, mayFirstFormat);
}

void Window::createPreviewGroupBox() {
    previewGroupBox = new QGroupBox(tr("Превью"));
    calendar = new QCalendarWidget;
    calendar->setMinimumDate(QDate(1900, 1, 1));
    calendar->setMaximumDate(QDate(3000, 1, 1));
    calendar->setGridVisible(true);
    connect(calendar, &QCalendarWidget::currentPageChanged, this, &Window::reformatCalendarPage);
    previewLayout = new QGridLayout;
    previewLayout->addWidget(calendar, 0, 0, Qt::AlignCenter);
    previewGroupBox->setLayout(previewLayout);
}

void Window::createGeneralOptionsGroupBox() {
    generalOptionsGroupBox = new QGroupBox(tr("Основные опции"));

    localeCombo = new QComboBox;
    int curLocaleIndex = -1;
    int index = 0;
    for (int _lang = QLocale::C; _lang <= QLocale::LastLanguage; ++_lang) {
        QLocale::Language lang = static_cast<QLocale::Language>(_lang);
        const auto locales =
            QLocale::matchingLocales(lang, QLocale::AnyScript, QLocale::AnyTerritory);
        for (auto loc : locales) {
            QString label = QLocale::languageToString(lang);
            auto territory = loc.territory();
            label += QLatin1Char('/');
            label += QLocale::territoryToString(territory);
            if (locale().language() == lang && locale().territory() == territory)
                curLocaleIndex = index;
            localeCombo->addItem(label, loc);
            ++index;
        }
    }
    if (curLocaleIndex != -1)
        localeCombo->setCurrentIndex(curLocaleIndex);
    localeLabel = new QLabel(tr("&Местоположение"));
    localeLabel->setBuddy(localeCombo);

    firstDayCombo = new QComboBox;
    firstDayCombo->addItem(tr("Понедельник"), Qt::Monday);
    firstDayCombo->addItem(tr("Вторник"), Qt::Tuesday);
    firstDayCombo->addItem(tr("Среда"), Qt::Wednesday);
    firstDayCombo->addItem(tr("Четверг"), Qt::Thursday);
    firstDayCombo->addItem(tr("Пятница"), Qt::Friday);
    firstDayCombo->addItem(tr("Суббота"), Qt::Saturday);
    firstDayCombo->addItem(tr("Воскресенье"), Qt::Sunday);

    firstDayLabel = new QLabel(tr("Неделя начинается:"));
    firstDayLabel->setBuddy(firstDayCombo);

    selectionModeCombo = new QComboBox;
    selectionModeCombo->addItem(tr("Одиночный выбор"),
                                QCalendarWidget::SingleSelection);
    selectionModeCombo->addItem(tr("None"), QCalendarWidget::NoSelection);

    selectionModeLabel = new QLabel(tr("&Режим выбора:"));
    selectionModeLabel->setBuddy(selectionModeCombo);

    gridCheckBox = new QCheckBox(tr("&Сетка"));
    gridCheckBox->setChecked(calendar->isGridVisible());

    navigationCheckBox = new QCheckBox(tr("&Панель навигации"));
    navigationCheckBox->setChecked(true);

    horizontalHeaderCombo = new QComboBox;
    horizontalHeaderCombo->addItem(tr("Одна буква"),
                                   QCalendarWidget::SingleLetterDayNames);
    horizontalHeaderCombo->addItem(tr("Короткие имена"),
                                   QCalendarWidget::ShortDayNames);
    horizontalHeaderCombo->addItem(tr("Нет"),
                                   QCalendarWidget::NoHorizontalHeader);
    horizontalHeaderCombo->setCurrentIndex(1);

    horizontalHeaderLabel = new QLabel(tr("&Горизонтальный заголовок:"));
    horizontalHeaderLabel->setBuddy(horizontalHeaderCombo);

    verticalHeaderCombo = new QComboBox;
    verticalHeaderCombo->addItem(tr("Номера недель ИСО"),
                                 QCalendarWidget::ISOWeekNumbers);
    verticalHeaderCombo->addItem(tr("Нет"), QCalendarWidget::NoVerticalHeader);

    verticalHeaderLabel = new QLabel(tr("&Вертикальный заголовок:"));
    verticalHeaderLabel->setBuddy(verticalHeaderCombo);

    connect(localeCombo, &QComboBox::currentIndexChanged,
            this, &Window::localeChanged);
    connect(firstDayCombo, &QComboBox::currentIndexChanged,
            this, &Window::firstDayChanged);
    connect(selectionModeCombo, &QComboBox::currentIndexChanged,
            this, &Window::selectionModeChanged);
    connect(gridCheckBox, &QCheckBox::toggled,
            calendar, &QCalendarWidget::setGridVisible);
    connect(navigationCheckBox, &QCheckBox::toggled,
            calendar, &QCalendarWidget::setNavigationBarVisible);
    connect(horizontalHeaderCombo, &QComboBox::currentIndexChanged,
            this, &Window::horizontalHeaderChanged);
    connect(verticalHeaderCombo, &QComboBox::currentIndexChanged,
            this, &Window::verticalHeaderChanged);

    QHBoxLayout *checkBoxLayout = new QHBoxLayout;
    checkBoxLayout->addWidget(gridCheckBox);
    checkBoxLayout->addStretch();
    checkBoxLayout->addWidget(navigationCheckBox);

    QGridLayout *outerLayout = new QGridLayout;
    outerLayout->addWidget(localeLabel, 0, 0);
    outerLayout->addWidget(localeCombo, 0, 1);
    outerLayout->addWidget(firstDayLabel, 1, 0);
    outerLayout->addWidget(firstDayCombo, 1, 1);
    outerLayout->addWidget(selectionModeLabel, 2, 0);
    outerLayout->addWidget(selectionModeCombo, 2, 1);
    outerLayout->addLayout(checkBoxLayout, 3, 0, 1, 2);
    outerLayout->addWidget(horizontalHeaderLabel, 4, 0);
    outerLayout->addWidget(horizontalHeaderCombo, 4, 1);
    outerLayout->addWidget(verticalHeaderLabel, 5, 0);
    outerLayout->addWidget(verticalHeaderCombo, 5, 1);
    generalOptionsGroupBox->setLayout(outerLayout);

    firstDayChanged(firstDayCombo->currentIndex());
    selectionModeChanged(selectionModeCombo->currentIndex());
    horizontalHeaderChanged(horizontalHeaderCombo->currentIndex());
    verticalHeaderChanged(verticalHeaderCombo->currentIndex());
}

void Window::createDatesGroupBox()
{
    datesGroupBox = new QGroupBox(tr("Даты"));

    minimumDateEdit = new QDateEdit;
    minimumDateEdit->setDisplayFormat("d MMM yyyy");
    minimumDateEdit->setDateRange(calendar->minimumDate(),
                                  calendar->maximumDate());
    minimumDateEdit->setDate(calendar->minimumDate());

    minimumDateLabel = new QLabel(tr("&Минимальная дата:"));
    minimumDateLabel->setBuddy(minimumDateEdit);

    currentDateEdit = new QDateEdit;
    currentDateEdit->setDisplayFormat("d MMM yyyy");
    currentDateEdit->setDate(calendar->selectedDate());
    currentDateEdit->setDateRange(calendar->minimumDate(),
                                  calendar->maximumDate());

    currentDateLabel = new QLabel(tr("&Настоящая дата:"));
    currentDateLabel->setBuddy(currentDateEdit);

    maximumDateEdit = new QDateEdit;
    maximumDateEdit->setDisplayFormat("d MMM yyyy");
    maximumDateEdit->setDateRange(calendar->minimumDate(),
                                  calendar->maximumDate());
    maximumDateEdit->setDate(calendar->maximumDate());

    maximumDateLabel = new QLabel(tr("Максимальная дата:"));
    maximumDateLabel->setBuddy(maximumDateEdit);

    connect(currentDateEdit, &QDateEdit::dateChanged,
            calendar, &QCalendarWidget::setSelectedDate);
    connect(calendar, &QCalendarWidget::selectionChanged,
            this, &Window::selectedDateChanged);
    connect(minimumDateEdit, &QDateEdit::dateChanged,
            this, &Window::minimumDateChanged);
    connect(maximumDateEdit, &QDateEdit::dateChanged,
            this, &Window::maximumDateChanged);

    QGridLayout *dateBoxLayout = new QGridLayout;
    dateBoxLayout->addWidget(currentDateLabel, 1, 0);
    dateBoxLayout->addWidget(currentDateEdit, 1, 1);
    dateBoxLayout->addWidget(minimumDateLabel, 0, 0);
    dateBoxLayout->addWidget(minimumDateEdit, 0, 1);
    dateBoxLayout->addWidget(maximumDateLabel, 2, 0);
    dateBoxLayout->addWidget(maximumDateEdit, 2, 1);
    dateBoxLayout->setRowStretch(3, 1);

    datesGroupBox->setLayout(dateBoxLayout);
}

void Window::createTextFormatsGroupBox()
{
    textFormatsGroupBox = new QGroupBox(tr("Формат текста"));

    weekdayColorCombo = createColorComboBox();
    weekdayColorCombo->setCurrentIndex(
        weekdayColorCombo->findText(tr("Чёрный")));

    weekdayColorLabel = new QLabel(tr("&Цвет дней недели:"));
    weekdayColorLabel->setBuddy(weekdayColorCombo);

    weekendColorCombo = createColorComboBox();
    weekendColorCombo->setCurrentIndex(
        weekendColorCombo->findText(tr("Красный")));

    weekendColorLabel = new QLabel(tr("Цвет выходных:"));
    weekendColorLabel->setBuddy(weekendColorCombo);

    headerTextFormatCombo = new QComboBox;
    headerTextFormatCombo->addItem(tr("Жирный"));
    headerTextFormatCombo->addItem(tr("Курсив"));
    headerTextFormatCombo->addItem(tr("Обычный"));

    headerTextFormatLabel = new QLabel(tr("&Заголовочный текст:"));
    headerTextFormatLabel->setBuddy(headerTextFormatCombo);

    firstFridayCheckBox = new QCheckBox(tr("&Первая пятница в синем"));

    mayFirstCheckBox = new QCheckBox(tr("Первое мая в красном"));

    connect(weekdayColorCombo, &QComboBox::currentIndexChanged,
            this, &Window::weekdayFormatChanged);
    connect(weekdayColorCombo, &QComboBox::currentIndexChanged,
            this, &Window::reformatCalendarPage);
    connect(weekendColorCombo, &QComboBox::currentIndexChanged,
            this, &Window::weekendFormatChanged);
    connect(weekendColorCombo, &QComboBox::currentIndexChanged,
            this, &Window::reformatCalendarPage);
    connect(headerTextFormatCombo, &QComboBox::currentIndexChanged,
            this, &Window::reformatHeaders);
    connect(mayFirstCheckBox, &QCheckBox::toggled,
            this, &Window::reformatCalendarPage);

    QHBoxLayout *checkBoxLayout = new QHBoxLayout;
    checkBoxLayout->addStretch();
    checkBoxLayout->addWidget(mayFirstCheckBox);

    QGridLayout *outerLayout = new QGridLayout;
    outerLayout->addWidget(weekdayColorLabel, 0, 0);
    outerLayout->addWidget(weekdayColorCombo, 0, 1);
    outerLayout->addWidget(weekendColorLabel, 1, 0);
    outerLayout->addWidget(weekendColorCombo, 1, 1);
    outerLayout->addWidget(headerTextFormatLabel, 2, 0);
    outerLayout->addWidget(headerTextFormatCombo, 2, 1);
    outerLayout->addLayout(checkBoxLayout, 3, 0, 1, 2);
    textFormatsGroupBox->setLayout(outerLayout);

    weekdayFormatChanged();
    weekendFormatChanged();
    reformatHeaders();
    reformatCalendarPage();
}

QComboBox *Window::createColorComboBox() {
    QComboBox *comboBox = new QComboBox;
    comboBox->addItem(tr("Красный"), QColor(Qt::red));
    comboBox->addItem(tr("Синий"), QColor(Qt::blue));
    comboBox->addItem(tr("Чёрный"), QColor(Qt::black));
    comboBox->addItem(tr("Пурпурный"), QColor(Qt::magenta));
    return comboBox;
}
